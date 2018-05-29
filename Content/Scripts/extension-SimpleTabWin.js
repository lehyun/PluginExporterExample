'use strict';

function main() 
{    
    const UMG = require('UMG');
    const I = require('instantiator');
    const EMaker = require('editor-maker');
    const Assert = require('assert');

    // '// 'Unreal.js/Samples' 메뉴그룹이 없다면 생성한다. Samples' 메뉴그룹이 없다면 생성한다. 
    if(!global.editorGroup){
        global.editorGroup = JavascriptEditorLibrary.GetGroup('Root').AddGroup('Unreal.js').AddGroup('Samples');
    }

    function showWindow(isModal) {
        let title;
        let msg;
        if (isModal) {
            title = 'Modal Window';
            msg = 'This is a modal window. \nThis is a modal window. \nThis is a modal window. \nThis is a modal window.';
        }
        else {
            title = 'Normal Window';
            msg = 'This is a normal window. \nThis is a normal window. \nThis is a normal window. \nThis is a normal window.';
        }

        /** @type {TextBlock} */
        let design = UMG(JavascriptWindow, 
            {
                AutoCenter:EJavascriptAutoCenter.PrimaryWorkArea,
                SizingRule:EJavascriptSizingRule.Autosized, 
                SupportsMaximize:false,
                Title:title
            },
            UMG.text(
                {
                    MinDesiredWidth:200,
                    bAutoWrapText:true
                }, msg)
        );
        if (isModal)
        {
            /** Modal 윈도우 @type {JavascriptSlateWidget} */
            I(design).TakeWidget().EditorAddModalWindow();
        }
        else
        {
            /** 일반 윈도우 @type {JavascriptSlateWidget} */
            I(design).TakeWidget().AddWindow(true);
        }
    }

    /** 루트 컨텐츠 위젯: @type {Widget} */
    let rootContent = null;
    /** 탭 스포너: @type {JavascriptEditorTab} */
    let rootWindowTab = null;
    function registerMainWindow() {
        rootWindowTab = EMaker.tabSpawner(
            // 옵션 파라미터
            {
                DisplayName:'Simple Tab Win',
                TabId: 'SimpleTabWin@',
                Role: EJavascriptTabRole.MajorTab,
                Group: global.editorGroup
            },
            // main 파라미터; 컨텐츠 구성 함수
            () => {
                console.log('1. 컨텐츠 구성 시작');
                // 메뉴 클릭시, TakeWidget()에서 
                let content = I(
                    UMG.div(
                        {
                            $link:elem => {
                                console.log('2. UI 생성자');
                                rootContent = elem;
                            },
                            $unlink:elem => {
                                console.log('4. UI 소멸자');
                                rootContent = null;
                            }
                        }, 
                        UMG.text({},'Click button to open a window'),
                        UMG(Button, {
                            OnClicked: () => {
                                showWindow(false);
                            }
                        }, 'Open Normal Window'),
                        UMG(Button, {
                            OnClicked: () => {
                                showWindow(true);
                            }
                        }, 'Open Modal Window'),
                        UMG(Button, {
                            OnClicked: () => {
                                if (rootWindowTab) {
                                    console.log(`${typeof rootWindowTab}`);
                                    rootWindowTab.Close();
                                }
                            }
                        }, 'Close Me')
                    )
                );
                console.log('3. 컨텐츠 구성 완료');
                // 생성한 컨텐츠는 루트 컨텐츠!
                Assert(content == rootContent);
                return content;
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
    require('bootstrap')('extension-SimpleTabWin');
}