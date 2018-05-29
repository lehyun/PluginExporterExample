'use strict';

function main()
{
    const UMG = require('UMG');
    const I = require('instantiator');
    const EMaker = require('editor-maker');
    
    // 'Unreal.js/Samples' 메뉴그룹이 없다면 생성한다. 
    if(!global.editorGroup){
        global.editorGroup = JavascriptEditorLibrary.GetGroup('Root').AddGroup('Unreal.js').AddGroup('Samples');
    }

    //get engine
    const GEngine = Root.GetEngine();

    const SmallFontSetting = {
        FontObject:GEngine.SmallFont,
        Size:10
    };
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

    // 자바스크립트 기초: let vs var
    function testLetVsVar() {
        // let vs var
        // let allows you to declare variables that are limited in scope to the block, statement, or expression on which it is used. 
        // This is unlike the var keyword, which defines a variable globally, or locally to an entire function regardless of block scope
        let x = 1;
        if (x == 1) {
            let x = 2;
            console.log(`x = ${x}`); // x = 2
        }
        console.log(`x = ${x}`); // x = 1

        var y = 1;
        if (y == 1) {
            var y = 2;
            console.log(`y = ${y}`); // y = 2
        }
        console.log(`y = ${y}`); // y = 2
    }

    function registerMainWindow() {
        
        // 게임 컨텐츠 경로 얻어오기
        let contentRootPath = Root.GetDir('GameContent');
        //Editor window with tab simple template
        EMaker.tabSpawner(
            {
                DisplayName:'Sample Text',
                TabId: 'Sampvarext@',
                Role: EJavascriptTabRole.MajorTab,
                Group: global.editorGroup
            },
            () => {
                // let vs var 테스트
                testLetVsVar();

                let widget = I(
                    UMG.div(
                        {},
                        //suger-syntaxes
                        UMG.text({},'UMG.text()'),
                        UMG(TextBlock,{Text:'UMG(TextBlock,{Text:\'text\'})'}),
                        //font settings
                        UMG.text({Font:TinyFontSetting},'TinyFont text (10)'),
                        UMG.text({Font:SmallFontSetting},'SmallFont text (10)'),
                        UMG.text({Font:MediumFontSetting},'MediumFont text (10)'),
                        UMG.text({Font:SubtitleFontSetting},'SubtitleFont text (10)'),
                        UMG.text({Font:LargeFontSetting},'LargeFont text (10)'),
                        UMG.text({Font:TinyFontSetting}, `ContentRootPath=[${contentRootPath}]`)
                    )
                );
                return widget;
            }
        );
    }

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
    require('bootstrap')('extension-Text');
}