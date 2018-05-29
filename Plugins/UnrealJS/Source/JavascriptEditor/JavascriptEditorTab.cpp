PRAGMA_DISABLE_SHADOW_VARIABLE_WARNINGS

#include "JavascriptEditorTab.h"
#if WITH_EDITOR
#include "Button.h"
#include "SSpacer.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#endif

UJavascriptEditorTab::UJavascriptEditorTab(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
#if WITH_EDITOR
, bIsNomad(true), bRegistered(false)
#endif
{	
}

#if WITH_EDITOR
TSharedPtr<SDockTab> UJavascriptEditorTab::MajorTab;

void UJavascriptEditorTab::BeginDestroy()
{
	Super::BeginDestroy();

	Discard();
}

void UJavascriptEditorTab::Commit()
{
	Discard();

	IJavascriptEditorModule::Get().AddExtension(this);
	
	bRegistered = true;
}

void UJavascriptEditorTab::Discard()
{
	if (bRegistered)
	{
		IJavascriptEditorModule::Get().RemoveExtension(this);
	}

	bRegistered = false;
}

UWidget* UJavascriptEditorTab::TakeWidget(UObject* Context)
{
	if (OnSpawnTab.IsBound())
	{
		UWidget* Widget = OnSpawnTab.Execute(Context);
		if (Widget) return Widget;
	}
	return NewObject<UButton>();
}

void UJavascriptEditorTab::TabActivatedCallback(TSharedRef<SDockTab> Tab, ETabActivationCause Cause)
{
	if (OnTabActivatedCallback.IsBound())
	{
		OnTabActivatedCallback.Execute(Tab->GetLayoutIdentifier().ToString(), TEnumAsByte<EJavasriptTabActivationCause::Type>((uint8)Cause));
	}
}

struct FJavascriptEditorTabTracker : public FGCObject
{
	TArray<UJavascriptEditorTab*> Spawners;
	TArray<UWidget*> Widgets;
	TArray<TWeakPtr<SDockTab>> Tabs;

	void RequestCloseTab(UWidget* Widget)
	{
		for (int Index = Tabs.Num() - 1; Index >= 0; --Index)
		{
			if (Widgets[Index] == Widget)
			{
				// HL: fixed the issue that it closes only the widget remaining the dock tab behind.
				//RemoveIndex(Index);
				Tabs[Index].Pin()->RequestCloseTab();
				break;
			}
		}
	}
	void RequestCloseTab(UJavascriptEditorTab* Spawner)
	{
		for (int Index = Tabs.Num() - 1; Index >= 0; --Index)
		{
			if (Spawners[Index] == Spawner)
			{
				Tabs[Index].Pin()->RequestCloseTab();
				break;
			}
		}
	}

	// callback when a tab is closed.
	void OnTabClosed(TSharedRef<SDockTab> Tab)
	{
		Tab->SetContent(SNew(SSpacer));

		for (int Index = Tabs.Num() - 1; Index >= 0; --Index)
		{
			if (Tabs[Index] == Tab)
			{
				RemoveIndex(Index);
				break;
			}
		}		
	}

	void RemoveIndex(int Index)
	{
		if (Tabs[Index].IsValid())
		{
			Tabs[Index].Pin()->SetContent(SNew(SSpacer));
		}
		// HL: fixed an issue that the editor crashes while closing on a certain circumstance
		auto Spawner = Spawners[Index];
		auto Widget = Widgets[Index];
		Spawners.RemoveAt(Index);
		Tabs.RemoveAt(Index);
		Widgets.RemoveAt(Index);
		
		if (Spawner->OnCloseTab.IsBound())
			Spawner->OnCloseTab.ExecuteIfBound(Widget);
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		for (int Index = Tabs.Num() - 1; Index >= 0; --Index)
		{
			if (!Tabs[Index].IsValid())
			{
				RemoveIndex(Index);
			}
		}

		Collector.AddReferencedObjects(Widgets);
		Collector.AddReferencedObjects(Spawners);
	}

	bool bInit{ false };

	void MaybeInit()
	{
		if (bInit) return;

		bInit = true;

		FEditorDelegates::OnShutdownPostPackagesSaved.AddLambda([this]() {
			FEditorScriptExecutionGuard ScriptGuard;

			while (Tabs.Num())
			{
				RemoveIndex(Tabs.Num() - 1);
			}
		});
	}

	void Add(UJavascriptEditorTab* Spawner, UWidget* Widget, TWeakPtr<SDockTab> Tab)
	{
		MaybeInit();

		Spawners.Add(Spawner);
		Widgets.Add(Widget);
		Tabs.Add(Tab);
	}

	TSharedPtr<SDockTab> Find(UWidget* Widget)
	{
		for (int Index = Widgets.Num() - 1; Index >= 0; --Index)
		{
			for (auto p = Widget; p; p = p->GetParent())
			{
				if (p == Widgets[Index])
				{
					return Tabs[Index].Pin();
				}
			}
		}

		return TSharedPtr<SDockTab>();
	}
} GEditorTabTracker;

TSharedPtr<SDockTab> UJavascriptEditorTab::FindDocktab(UWidget* Widget)
{
	// under construction?
	if (MajorTab.IsValid()) return MajorTab;

	return GEditorTabTracker.Find(Widget);
}

void UJavascriptEditorTab::CloseTab(UWidget* Widget)
{
	GEditorTabTracker.RequestCloseTab(Widget);
}

void UJavascriptEditorTab::Close()
{
	GEditorTabTracker.RequestCloseTab(this);
}

void UJavascriptEditorTab::Show()
{
	auto TabManager = CachedTabManager.Pin();
	if (TabManager.IsValid()) TabManager->InvokeTab(TabId);
}

void UJavascriptEditorTab::Register(TSharedRef<FTabManager> TabManager, UObject* Context, TSharedRef<FWorkspaceItem> Group)
{
	FSlateIcon Icon(FEditorStyle::GetStyleSetName(), "DeviceDetails.Tabs.ProfileEditor");

	const bool bGlobal = TabManager == FGlobalTabmanager::Get();
	CachedTabManager = TabManager;

	auto Lambda = FOnSpawnTab::CreateLambda([this, Context, bGlobal](const FSpawnTabArgs& SpawnTabArgs){
		auto Widget = this->TakeWidget(Context);

		const TSharedRef<SDockTab> MajorTab = SNew(SDockTab)
			.TabRole(ETabRole(Role.GetValue()))
			.OnTabClosed_Lambda([](TSharedRef<SDockTab> ClosedTab) {
				GEditorTabTracker.OnTabClosed(ClosedTab);
			});

		GEditorTabTracker.Add(this, Widget, MajorTab);

		auto OldTab = UJavascriptEditorTab::MajorTab;
		UJavascriptEditorTab::MajorTab = MajorTab;		 
		
		MajorTab->SetContent(Widget->TakeWidget());
		MajorTab->SetOnTabActivated(FOnTabActivatedCallback::CreateLambda([this](TSharedRef<SDockTab> Tab, ETabActivationCause Cause) {
			this->TabActivatedCallback(Tab, Cause);
		}));
		UJavascriptEditorTab::MajorTab = OldTab;

		return MajorTab;
	});	

	auto& SpawnerEntry = bIsNomad && bGlobal ? FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabId, Lambda) : TabManager->RegisterTabSpawner(TabId, Lambda);
	SpawnerEntry
		.SetDisplayName(DisplayName)
		.SetIcon(Icon)
		.SetGroup(Group);	
}

void UJavascriptEditorTab::Unregister(TSharedRef<FTabManager> TabManager)
{
	if (bIsNomad && TabManager == FGlobalTabmanager::Get())
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
	}
	else
	{
		TabManager->UnregisterTabSpawner(TabId);
	}
	CachedTabManager = nullptr;
}

void UJavascriptEditorTab::Register()
{	
	Register(FGlobalTabmanager::Get(), nullptr, Group.Handle.IsValid() ? Group.Handle.ToSharedRef() : WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory());
}

void UJavascriptEditorTab::Unregister()
{
	Unregister(FGlobalTabmanager::Get());
}
#endif

PRAGMA_ENABLE_SHADOW_VARIABLE_WARNINGS