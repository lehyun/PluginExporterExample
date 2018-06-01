
'use strict';

function main(){
    const UMG = require('UMG');
    const I = require('instantiator');
    const EMaker = require('editor-maker');



    // 'Unreal.js/Samples' 메뉴그룹이 없다면 생성한다. 
    if(!global.editorGroup){
        global.editorGroup = JavascriptEditorLibrary.GetGroup('Root').AddGroup('Unreal.js').AddGroup('Samples');
    }

    // 'Tabs' 메뉴그룹이 없다면 생성한다.
    if(!global.tabGroup){
        global.tabGroup = JavascriptWorkspaceItem.AddGroup(JavascriptWorkspaceItem.GetGroup('Root'), 'Tabs');
    }

    let rootContent = null;
    let rootWindowTab = null;
    let tabManager = new JavascriptEditorTabManager(JavascriptLibrary.CreatePackage(null,'/Script/Javascript'));

    /** @type {JavascriptEditorTab[]} */
    let tabs = [
        EMaker.tab(
            // opts: 옵션
            {
                TabId:'Tab1',
                // MajorTab : 기본 탭에서 패널로 도킹 수 없다.
                // NomadTab : 다른 NomadTab의 모서리에 도킹하거나 탭을 숨겨 패널화할 수 있다.
                // DocumentTab, PanelTab: 같은 그룹 안에서만 도킹 가능.
                Role:EJavascriptTabRole.MajorTab,
                DisplayName:'Tab1',
                Group:global.tabGroup,
                bIsNomad:true
            },
            // tab_fn: OnSpawnTab 델리게이트
            (context) => {
                if (context) {
                    console.log(`context = ${context.GetObjectName()}, ${context.GetDisplayName()}, ${context.GetPathName()}`);
                }

                let elemFromLink = null;
                // 위젯 리턴
                let widget = I(
                    UMG.div(
                        {
                            // UI 생성시 호출
                            $link:elem => {
                                // window == widget 같다
                                elemFromLink = elem;
                            }
                        },
                        UMG.span({}, UMG.text({}, 'Select Tab')),
                        UMG(Button, {
                            OnClicked:_ => { 
                                console.log(`Button#1 clicked`);
                                // link.elem과 widget은 같다
                                if (elemFromLink == widget) {
                                    console.log('elemFromLink == widget');
                                }

                                // 특정 탭 닫기 - 성공
                                // 플러그인 버그로 위젯만 삭제됨. 
                                // JavascriptEditorTab.cpp:78 수정하여 해결
                                //tabs[0].CloseTab(widget);
                                // 탭에서 닫기 함수 추가 - 성공
                                // JavascriptEditorTab.cpp:199
                                //tabs[1].Close();
                                tabs[1].Show();
                            }
                        }, 'Open Tab#2'),
                        UMG(Button, {
                            OnClicked:_ => { 
                                console.log(`Button#2 clicked`);
                                tabs[2].Show();
                            }
                        }, 'Open Tab#3'),
                        UMG(Button, {
                            OnClicked:_ => { 
                                console.log(`Button#3 clicked`);
                                tabs[3].Show();
                            }
                        }, 'Open Tab#4')
                    )
                );
                return widget;
            },
            // del_fn: OnCloseTab 델리게이트
            (widget) => {
                if (widget) {
                    console.log(`widget = ${widget.GetObjectName()}, ${widget.GetDisplayName()}, ${widget.GetPathName()}`);
                }
                widget = null;

                // 0번 탭이 닫히면 모든 탭이 닫히도록
                //--> 성공: 이렇게 하면 루트 윈도우 탭을 닫는다
                rootWindowTab.Close();

                //--> 성공: 이렇게 하면 루트 탭과 도킹영역은 남기고, 모든 하위 탭만 날린다.
                //tabManager.RemoveFromParent();

                // --> 에러: 이렇게 하면 루트 윈도우의 컨텐츠만 날라가서 나중에 창 닫을때 에러발생
                //rootContent.RemoveFromParent();
            }
        ),
        EMaker.tab(
            {
                TabId:'Tab2',
                Role:EJavascriptTabRole.PanelTab,
                DisplayName:'Tab2',
                Group: global.tabGroup
            },
            (context) => {
                if (context) {
                    console.log(`context = ${context.GetObjectName()}, ${context.GetDisplayName()}, ${context.GetPathName()}`);
                }

                let widget = I(UMG.text({},EJavascriptTabRole.PanelTab));
                return widget;
            },
            (widget) => {
                if (widget) {
                    console.log(`widget = ${widget.GetObjectName()}, ${widget.GetDisplayName()}, ${widget.GetPathName()}`);
                }
                widget = null;
            }
        ),
        EMaker.tab(
            {
                TabId:'Tab3',
                Role:EJavascriptTabRole.DocumentTab,
                DisplayName:'Tab3',
                Group: global.tabGroup
            },
            (context) => {
                if (context) {
                    console.log(`context = ${context.GetObjectName()}, ${context.GetDisplayName()}, ${context.GetPathName()}`);
                }

                return I(UMG.text({},EJavascriptTabRole.DocumentTab))
            },
            (widget) => {
                if (widget) {
                    console.log(`widget = ${widget.GetObjectName()}, ${widget.GetDisplayName()}, ${widget.GetPathName()}`);
                }
                widget = null;
            }
        ),
        EMaker.tab(
            {
                TabId:'Tab4',
                Role:EJavascriptTabRole.NomadTab,
                DisplayName:'Tab4',
                Group: global.tabGroup
            },
            (context) => {
                if (context) {
                    console.log(`context = ${context.GetObjectName()}, ${context.GetDisplayName()}, ${context.GetPathName()}`);
                }

                return I(UMG.text({},EJavascriptTabRole.NomadTab))
            },
            (widget) => {
                if (widget) {
                    console.log(`widget = ${widget.GetObjectName()}, ${widget.GetDisplayName()}, ${widget.GetPathName()}`);
                }
                widget = null;
            }
        )
    ]

    /**
     * Tab Manager layout JSON
     * 
     * check 
     * - [FLayout](https://docs.unrealengine.com/latest/INT/API/Runtime/Slate/Framework/Docking/FTabManager/FLayout/index.html)
     * - [FArea](https://docs.unrealengine.com/latest/INT/API/Runtime/Slate/Framework/Docking/FTabManager/FArea/index.html)
     * - [FLayoutNode](https://docs.unrealengine.com/latest/INT/API/Runtime/Slate/Framework/Docking/FTabManager/FLayoutNode/index.html)
     * - [FStack](https://docs.unrealengine.com/latest/INT/API/Runtime/Slate/Framework/Docking/FTabManager/FStack/index.html)
     * - [FSplitter](https://docs.unrealengine.com/latest/INT/API/Runtime/Slate/Framework/Docking/FTabManager/FSplitter/index.html)
     * - [ETabState::Type](https://docs.unrealengine.com/latest/INT/API/Runtime/Slate/Framework/Docking/ETabState__Type/index.html)
     */
    let tabLayout = {
        // Type = 'Area', 'Splitter', 'Stack', 'Layout'
        Type:'Layout',
        Name:'Layout',
        PrimaryAreaIndex: 0,
        Areas: [
            {
                Type: 'Area',
                // 'Orient_Vertical', 'Orient_Horizontal'
                Orientation: 'Orient_Horizontal',
                //Orientation: 'Orient_Vertical',
                
                // EWindowPlacement : 'Placement_NoWindow', 'Placement_Automatic', 'Placement_Specified'
                // Placement_NoWindow: 프라이머리 윈도우. 반드시 1개 존재해야 함. 위치/사이즈 자유.
                // Placement_Automatic: 보조 윈도우. 위치 자유, 사이즈는 고정값 (WindowSize_X, WindowSize_Y)
                // Placement_Specified: 보조 윈도우. 위치, 사이즈, 최대화 고정값 (WindowPosition_X, WindowPosition_Y, WindowSize_X, WindowSize_Y, bIsMaximized)
                WindowPlacement: 'Placement_NoWindow',
                SizeCoefficient: 1,
                Nodes:[
                    {
                        Type: 'Stack',
                        SizeCoefficient : '0.2',
                        // true이면 탭을 숨긴다
                        HideTabWell: true,
                        Tabs: [
                            {                                
                                TabId: 'Tab1',
                                TabState: 'OpenedTab'
                            }
                        ]
                    },
                    {
                        Type: 'Stack',
                        SizeCoefficient : '0.8',
                        HideTabWell: false,
                        // 포커스할 탭
                        ForegroundTabId: 'Tab4',
                        Tabs: [
                            {                                
                                TabId: 'Tab2',
                                TabState: 'OpenedTab'  
                            },
                            {                                
                                TabId: 'Tab3',
                                TabState: 'OpenedTab'
                            },
                            {                                
                                TabId: 'Tab4',
                                TabState: 'OpenedTab'  
                            }
                        ]
                    }
                ]
            }
            /*,
            {
                SizeCoefficient: 1,
                Type: 'Area',
                Orientation: 'Orient_Horizontal',
                WindowPlacement: 'Placement_Automatic',
                WindowPosition_X: 0,
                WindowPosition_Y: 21,
                WindowSize_X: 640,
                WindowSize_Y: 480,
                bIsMaximized: true,
                Nodes:[
                    {
                        Type: 'Stack',
                        SizeCoefficient : 1,
                        HideTabWell: false,
                        Tabs: [
                            {                                
                                TabId: 'Tab4',
                                TabState: 'OpenedTab'
                            }
                        ]
                    }
                ]
            }*/
        ]
    }

    // tabManager settings
    tabManager.Tabs = tabs;
    tabManager.Layout = JSON.stringify(tabLayout);

    // 메인 윈도우 등록
    /** @type {JavascriptEditorTab} */
    rootWindowTab = EMaker.tabSpawner(
        {
            DisplayName:'Sample Multi-Tab Window',
            TabId: 'Sampvarabs@',
            Role: EJavascriptTabRole.MajorTab,
            Group: global.editorGroup
        },
        () => I(
            UMG.div(
                {
                    $link:elem => {
                        // elem은 루트 UI 컨텐츠
                        console.log('UI 생성자 호출');
                        rootContent = elem;
                        elem.AddChild(tabManager).Size.SizeRule  = 'Fill'
                    },
                    $unlink:elem => {
                        console.log('UI 소멸자 호출');
                    }
                }
            )
        )
    );

    //deconstructor
    return () => {
        
    };
}

// bootstrap to initiate live-reloading dev env.
try {
    module.exports = () => {
        let cleanup = null

        // wait for map to be loaded.
        process.nextTick(() => cleanup = main());

        // live-reloadable function should return its cleanup function
        return () => cleanup();
    }
}
catch (e) {
    require('bootstrap')('extension-Tabs')
}