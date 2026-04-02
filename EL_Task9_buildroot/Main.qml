import QtQuick
import QtQuick.Controls

pragma ComponentBehavior: Bound

ApplicationWindow{
    id: mainWindow
    width: 500
    height: 400
    visible: true
    title: qsTr("Calculator")

    // Window Background Gradient
    readonly property color windowGradStart: "#0B0C1E"
    readonly property color windowGradEnd:   "#1A1040" 

    // Calculator Panel Background Gradient
    readonly property color calcGradStart: "#12132B" 
    readonly property color calcGradEnd:   "#1E1A45" 

    // Text Colors
    readonly property color textPrimary:   '#e7ebfa' 
    readonly property color textSecondary: '#798dca'
    readonly property color textAccent:    '#081535' 

    // Textoutput Container Gradient
    readonly property color outputGradStart: "#080A1C" 
    readonly property color outputGradEnd:   "#10102E" 

    // Number Button Gradient (0–9)
    readonly property color numBtnGradStart: "#1E2154" 
    readonly property color numBtnGradEnd:   "#2A2B6E" 

    // Operator Button Gradient (+  −  ×  ÷ %)
    readonly property color opBtnGradStart: '#261363' 
    readonly property color opBtnGradEnd:   '#311179' 

    // Equal Button Gradient (=)
    readonly property color eqBtnGradStart: '#b776f0' 
    readonly property color eqBtnGradEnd:   '#c56ad1' 

    // Clear / Reset Button Gradient (C  CE  ←)
    readonly property color clrBtnGradStart: "#C0392B" 
    readonly property color clrBtnGradEnd:   "#E8500A" 

    // Subtle border/glow colors
    readonly property color borderGlow:     "#3D3580"   
    readonly property color outputBorder:    "#2A2060" 

    Rectangle{
        anchors.fill: parent
        gradient: Gradient{
            orientation: Gradient.Vertical
            GradientStop{ position: 0.0; color: mainWindow.windowGradStart }
            GradientStop{ position: 1.0; color: mainWindow.windowGradEnd   }
        }
        
        Rectangle{
            id: calcPanel
            width: parent.width * 0.85
            height: parent.height * 0.85
            anchors.centerIn: parent
            radius: 20
            gradient: Gradient{
                orientation: Gradient.Vertical
                GradientStop{ position: 0.0; color: mainWindow.calcGradStart }
                GradientStop{ position: 1.0; color: mainWindow.calcGradEnd   }
            }
            border.color: mainWindow.borderGlow
            border.width: 3

            // Calculator Column
            Column{
                anchors.fill: parent
                anchors.topMargin:    parent.height * 0.08
                anchors.leftMargin:   parent.width  * 0.08
                anchors.rightMargin:  parent.width  * 0.08
                anchors.bottomMargin: parent.height * 0.08
                spacing: parent.height * 0.08

                // Textoutput for display
                Rectangle{
                    id: textOutputContainer
                    width: parent.width
                    height: parent.height * 0.17
                    radius: 10
                    gradient: Gradient{
                        orientation: Gradient.Vertical
                        GradientStop{ position: 0.0; color: mainWindow.outputGradStart }
                        GradientStop{ position: 1.0; color: mainWindow.outputGradEnd   }
                    }
                    border.color: mainWindow.outputBorder
                    border.width: 2

                    TextInput{
                        id: textOutput
                        anchors.fill: parent
                        anchors.margins: parent.height * 0.2
                        font.pixelSize: height * 0.65
                        color: mainWindow.textSecondary
                        font.family: "Consolas"
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                        text: ""
                        // Read-only -> buttons control input, not keyboard
                        readOnly: true
                        // Enable selection and copy
                        selectByMouse: true
                        selectionColor: mainWindow.textSecondary
                        selectedTextColor: mainWindow.textAccent

                        // Cursor hidden since it's read-only
                        cursorVisible: false
                    }
                }
                // Button Grid
                Grid{
                    id: buttonGrid
                    columns: 5
                    width: parent.width   // ← inherit exact Column width
                    spacing: parent.width * 0.02

                    // ← base btnW on Grid's OWN width, not calcPanel
                    readonly property real btnW: (width - (spacing * (columns - 1))) / columns
                    readonly property real btnH: (calcPanel.height * 0.69) / 5
                    // make an array of button labels and properties to loop through
                    property var buttons: [
                        // Row 1
                       { label: "C",  gradStart: mainWindow.clrBtnGradStart, gradEnd: mainWindow.clrBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "CE", gradStart: mainWindow.clrBtnGradStart, gradEnd: mainWindow.clrBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "←",  gradStart: mainWindow.clrBtnGradStart, gradEnd: mainWindow.clrBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "%",  gradStart: mainWindow.opBtnGradStart, gradEnd: mainWindow.opBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "÷",  gradStart: mainWindow.opBtnGradStart,  gradEnd: mainWindow.opBtnGradEnd, textColor: mainWindow.textPrimary },
                        // Row 2
                       { label: "7",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "8",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "9",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: ".",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "*",  gradStart: mainWindow.opBtnGradStart,  gradEnd: mainWindow.opBtnGradEnd, textColor: mainWindow.textPrimary },
                        // Row 3
                       { label: "4",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "5",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "6",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "0",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "-",  gradStart: mainWindow.opBtnGradStart,  gradEnd: mainWindow.opBtnGradEnd, textColor: mainWindow.textPrimary },
                        // Row 4
                       { label: "1",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "2",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "3",  gradStart: mainWindow.numBtnGradStart, gradEnd: mainWindow.numBtnGradEnd, textColor: mainWindow.textPrimary },
                       { label: "=",  gradStart: mainWindow.eqBtnGradStart,  gradEnd: mainWindow.eqBtnGradEnd, textColor: mainWindow.textAccent },
                       { label: "+",  gradStart: mainWindow.opBtnGradStart,  gradEnd: mainWindow.opBtnGradEnd, textColor: mainWindow.textPrimary }
                    ]
                    Repeater{
                        model: buttonGrid.buttons.length
                        MButton{
                            required property int index
                            label: buttonGrid.buttons[index].label
                            gradStart: buttonGrid.buttons[index].gradStart
                            gradEnd: buttonGrid.buttons[index].gradEnd
                            textColor: buttonGrid.buttons[index].textColor
                            cornerRadius: 10
                            width: buttonGrid.btnW
                            height: buttonGrid.btnH

                            onBtnClicked: mainWindow.handleInput(label)
                        }
                    }
                }
            }
        }
    }

    readonly property int maxInputLength: 16
    // Calculator Logic
    function handleInput(label){
        switch(label){
            // Clear all
            case "C":
                textOutput.text = ""
                break

            // Clear last entry (back to last operator)
            case "CE":
                if(textOutput.text === "Error")
                    textOutput.text = ""
                else{
                    var ops = ["+", "-", "*", "/", "*", "%"]
                    var expr = textOutput.text
                    var i = expr.length - 1
                    // walk back past the last number
                    while (i >= 0 && !ops.includes(expr[i])) i--
                    textOutput.text = expr.substring(0, i + 1)
                }
                break

            // Backspace 
            case "←":
                if(textOutput.text === "Error")
                    textOutput.text = ""
                else
                    textOutput.text = textOutput.text.slice(0, -1)
                break

            // Evaluate
            case "=":
                try{
                    var result = eval(textOutput.text.replace(/÷/g,  "/"))
                    // round to 5 decimal places if it's a float
                    if (typeof result === "number" && !Number.isInteger(result))
                        result = parseFloat(result.toFixed(5))
                    textOutput.text = isFinite(result) ? String(result) : "Error"
                } 
                catch(e){
                    textOutput.text = "Error"
                }
                break

            // Prevent duplicate operators
            case "+": case "-": case "*": case "÷": case "%":
                if (textOutput.text === "" || textOutput.text === "Error"){
                    textOutput.text = ""
                } 
                else{
                    var last = textOutput.text.slice(-1)
                    var isOp = ["+", "-", "*", "÷", "%"].includes(last)
                    if (!isOp)
                        textOutput.text += label
                }
                break

            // Prevent duplicate decimal point
            case ".":
                if(textOutput.text === "Error")
                    textOutput.text = ""
                else{
                    // find the last number segment after last operator
                    var segments = textOutput.text.split(/[\+\-x*÷]/)
                    var lastSeg  = segments[segments.length - 1]
                    if (!lastSeg.includes("."))
                        textOutput.text += "."
                }
                break

            // Numbers -> default append
            default:
                if(textOutput.text === "Error")
                    textOutput.text = label
                else if(textOutput.text === "" && label === "0" || textOutput.text.length >= maxInputLength)
                    break
                else
                    textOutput.text += label
                break
        }
    }
}