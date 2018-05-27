"use strict";

function main() {
    const UMG = require("UMG");
    const I = require('instantiator');
    const EMaker = require("editor-maker");
    //get engine
    const GEngine = Root.GetEngine();

    // 'Samples' 메뉴그룹이 없다면 생성한다.
    if (!global.editorGroup){
        global.editorGroup = JavascriptWorkspaceItem.AddGroup(JavascriptWorkspaceItem.GetGroup("Root"), "Samples");
    }

    const SmallFontSetting = {
        FontObject:GEngine.SmallFont,
        Size:10
    };
    
    const btnFColor = new LinearColor()
    btnFColor.R = btnFColor.G = btnFColor.B = 0;
    btnFColor.A = 100;

    const TinyFontSetting = {
        FontObject:GEngine.TinyFont,
        Size:10
    }
    const MediumFontSetting = {
        FontObject:GEngine.MediumFont,
        Size:10
    }
    const SubtitleFontSetting = {
        FontObject:GEngine.SubtitleFont,
        Size:10
    }
    const LargeFontSetting = {
        FontObject:GEngine.LargeFont,
        Size:10
    }

    //Editor window with tab simple template
    EMaker.tabSpawner(
        {
            DisplayName:"Sample Buttons",
            TabId: "SamplelButtons@",
            Role: EJavascriptTabRole.MajorTab,
            Group: global.editorGroup
        },
        () => I(
            UMG.div(
                {'slot.size.size-rule':ESlateSizeRule.Fill},
                //simplest button
                UMG(Button,{},"default button"),
                //simple button with font decoration
                UMG(Button,{ColorAndOpacity:btnFColor},UMG.text({Font:SmallFontSetting},"decorated text button")),
                //horizontal buttons
                UMG.span({}, UMG(Button,{ColorAndOpacity:btnFColor},"A"), UMG(Button,{ColorAndOpacity:btnFColor},"B")),
                //OnClicked event
                UMG(Button,{OnClicked:_ => {

                    console.log('--> button clicked');
                    try {
                        const Md5 = require('md5');
                        var md5hash = Md5('my message');
                        console.log(`md5('my message') = ${md5hash}`);

                        var test = new VictorySubtitleCue();
                        test.Time = 1234;
                        console.log(`Time = ${test.Time}`);

                        // 블루프린트 라이브러리 사용가능
                        var degInt = TKMathFunctionLibrary.NegateInt(2300);
                        console.log(`degInt = ${degInt}`);

                        var plusInt = CSWBlueprintUtilities.Plus(2948, 1101);
                        console.log(`plutInt = ${plusInt}`);
                    } 
                    catch (ex) {
                        console.error(ex.toString());
                    }
                    
                    let design = UMG(JavascriptWindow, 
                        {
                            SizingRule:'AutoSized', 
                            Title:'Modal Window'
                        },
                        UMG.text({Font:SmallFontSetting, Slot:{Size:{SizeRule:ESlateSizeRule.Fill},HorizontalAlignment:EHorizontalAlignment.HAlign_Center}},"button clicked.")
                    );
                    I(design).TakeWidget().EditorAddModalWindow();
                }, ColorAndOpacity:btnFColor},UMG.text({Font:SmallFontSetting},"Open ModalWindow"))
            )
        )
    )

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
        return () => cleanup()
    }
}
catch (e) {
    require('bootstrap')('extension-Button')
}