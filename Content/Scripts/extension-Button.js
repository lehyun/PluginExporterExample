/// <reference path="typings/ue.d.ts">/>
// In older versions of VS Code you needed to reference your typings like <reference path> for somelibrary.d.ts.
// With new version you need to initialize your project by creating jsconfig.json

'use strict';

function main() {
    const UMG = require('UMG');
    const I = require('instantiator');
    const EMaker = require('editor-maker');
    //get engine
    const GEngine = Root.GetEngine();

    // 'Unreal.js/Samples' 메뉴그룹이 없다면 생성한다. 
    if(!global.editorGroup){
        global.editorGroup = JavascriptEditorLibrary.GetGroup('Root').AddGroup('Unreal.js').AddGroup('Samples');
    }

    const SmallFontSetting = {
        FontObject:GEngine.SmallFont,
        Size:10
    };
    
    const btnFColor = new LinearColor(); {
        btnFColor.R = btnFColor.G = btnFColor.B = 0;
        btnFColor.A = 100;
    }

    const TinyFontSetting = {
        FontObject:GEngine.TinyFont,
        Size:10
    };
    const MediumFontSetting = {
        FontObject:GEngine.MediumFont,
        Size:10
    };
    const SubtitleFontSetting = {
        FontObject:GEngine.SubtitleFont,
        Size:10
    };
    const LargeFontSetting = {
        FontObject:GEngine.LargeFont,
        Size:10
    };

    function showTextPopup(msg) {
        let design = UMG(JavascriptWindow, 
            {
                SizingRule:'AutoSized', 
                Title:'Modal Window'
            },
            // EditableTextBox
            UMG.input(
                { 
                    Font:SmallFontSetting,
                    IsReadOnly:true,
                    MinimumDesiredWidth:400
                }, 
                msg
            )
        );
        I(design).TakeWidget().EditorAddModalWindow();
    }

    function createMainWidget() {
        let widget = I(
            // 행 시작
            UMG.div(
                // Nested된 옵션 파라미터를 설정하는 다양한 방법
                // 1) 스트링안에서 .으로 접근하여 설정
                {'Slot.Size.SizeRule':ESlateSizeRule.Fill},
                // 2) 객체를 구성하여 설정
                //{ Slot:{Size:{SizeRule:ESlateSizeRule.Fill}} },

                // 가장 단순한 버튼 (글씨색: White)
                UMG(Button,{},'Default Button'),
                // 글씨색: Black
                UMG(Button, {ColorAndOpacity:btnFColor},
                    UMG.text({Font:SmallFontSetting},'Decorated Text Button')
                ),
                // 열 시작
                UMG.span(
                    {
                        // Automatic이 기본값
                        'Slot.Size.SizeRule':ESlateSizeRule.Automatic,
                        // --> Fill로 하면 창 크기를 늘릴때, 버튼의 높이가 같이 늘어난다.
                        // 두 버튼 가운데 정렬
                        'Slot.HorizontalAlignment':EHorizontalAlignment.HAlign_Center
                    }, 
                    UMG(SizeBox,
                        {
                            // 최소 가로사이즈를 최대로 놓고, Fill로 설정하면, 
                            // 창사이즈에 따라 버튼 사이즈가 늘어난다.
                            MinDesiredWidth: 10000,
                            'Slot.Size.SizeRule':ESlateSizeRule.Fill
                        }, UMG(Button, {ColorAndOpacity:btnFColor}, 'A')
                    ),
                    UMG(SizeBox,
                        {
                            MinDesiredWidth: 10000,
                            'Slot.Size.SizeRule':ESlateSizeRule.Fill
                        }, UMG(Button, {ColorAndOpacity:btnFColor}, 'B')
                    )
                ),
                //OnClicked event
                UMG(Button,
                    {
                        OnClicked: () => {
                            console.log('--> button clicked');
                            try {
                                const Md5 = require('md5');
                                let md5hash = Md5('my message');
                                let text = '';
                                text += `md5('my message') = ${md5hash}\n`;

                                // 블루프린트 라이브러리 사용가능
                                //* 플러그인에 포함된 블루프린트 라이브러리를 사용할 경우, 
                                //* 플러그인 LoadingPhase를 'PreDefault'로 해주어야
                                //* V8엔진에 로드된다
                                let degInt = TKMathFunctionLibrary.NegateInt(2300);
                                text += `degInt = ${degInt}\n`;

                                let plusInt = CSWBlueprintUtilities.Plus(2948, 1101);
                                text += `plutInt = ${plusInt}\n`;
                                
                                console.log(text);
                                showTextPopup(text);
                            } 
                            catch (ex) {
                                console.error(ex.toString());
                            }
                        }, 
                        ColorAndOpacity:btnFColor 
                }, UMG.text({Font:SmallFontSetting},'Module Test'))
            )
        );
        return widget;
    }

    function registerMainWindow() {
        EMaker.tabSpawner(
            {
                DisplayName:'Sample Buttons',
                TabId: 'SamplelButtons@',
                Role: EJavascriptTabRole.MajorTab,
                Group: global.editorGroup
            },
            () => {
                return createMainWidget();
            }
        );
    }

    // 메인 윈도우 등록
    registerMainWindow();

    //deconstructor
    return () => {
        
    };
}

// bootstrap to initiate live-reloading dev env.
try {
    module.exports = () => {
        let cleanup = null;

        // wait for map to be loaded.
        process.nextTick(() => cleanup = main());

        // live-reloadable function should return its cleanup function
        return () => cleanup();
    }
}
catch (e) {
    require('bootstrap')('extension-Button');
}