var sentDir = 0;
savedDrawings = [];
// Get the modal
var modal = document.getElementById("popup");

// Get the button that opens the modal
var btn = document.getElementById("ledPopup");

var socket = io();

function buzz() {
    socket.emit('json', { data: 'buzz' });
};
function move(input1, direction) {
    if (input1 === 'form') {
        var speed = document.getElementById("speed").value;
    } else {
        var speed = input1;
    }
    socket.emit('json', { data: 'move', speed: speed, dir: direction })
}

const listener = (args) => {
    var response = document.getElementById("response");
    response.innerHTML = args;
}

socket.on("response", listener);

function lightSensorPoll() {
    socket.emit('lightSensorPoll');
}

const lightSensorListener = (args) => {
    var lightSensorSpan = document.getElementById("lightSensorSpan");
    lightSensorSpan.innerHTML = args;
}
setInterval(lightSensorPoll, 500);

socket.on("lightSensor", lightSensorListener);


function led(event) {
    var ledX = document.getElementById("ledX").value;
    var ledY = document.getElementById("ledY").value;
    socket.emit('led', { coords: [{ x: ledX, y: ledY }] })
    event.preventDefault();
}

// var form = document.getElementById('testForm');
// form.addEventListener('submit', led);


document.addEventListener('DOMContentLoaded', function (event) {
    var canvas = Snap('#canvas');
    // var square = canvas.rect(10,10, 10, 10);
    // square.click(clickCallback);
    listOfSquare = [];
    for (a = 0; a < 8; a++) {
        for (b = 0; b < 16; b++) {
            listOfSquare.push(canvas.rect(b * 20, a * 20, 20, 20));
            listOfSquare[(a * 16) + b].click(clickCallback);
            listOfSquare[(a * 16) + b].attr({ fill: 'white' })
        }
    }
})

var clickCallback = function (event) {
    socket.emit('led', { coords: [{ x: this.node.attributes.x.value / 20, y: this.node.attributes.y.value / 20 }] });
    if (this.node.attributes.fill.value === '#ffffff') {
        this.attr({ fill: 'blue' })
    } else {
        this.attr({ fill: 'white' })
    }
}

function clearScreen() {
    var canvas = Snap('#canvas');
    canvasChildren = canvas.node.children;
    socket.emit('clearScreen');
    /* for (var i = 0; i < listOfSquare.length; i++) {
        listOfSquare[i].attr({fill: 'white'})
    } */

    for (var i = 0; i < canvasChildren.length; i++) {
        if (canvasChildren[i].nodeName === 'rect') {
            console.log(canvasChildren[i].attributes.fill.value);
            canvas.node.children[i].attributes.fill.value = '#ffffff';
        }
    }
}

function saveDrawing() {
    thisDrawing = []
    if (localStorage.getItem('savedDrawing') == undefined) {
        localStorage.setItem('savedDrawing', JSON.stringify({ array: [] }));
        currentState = JSON.parse(localStorage.getItem('savedDrawing'));
    } else {
        currentState = JSON.parse(localStorage.getItem('savedDrawing'));
    }
    var canvas = Snap('#canvas');
    canvasChildren = canvas.node.children;
    for (var i = 0; i < canvasChildren.length; i++) {
        if (canvasChildren[i].nodeName === 'rect') {
            if (canvasChildren[i].attributes.fill.value === '#0000ff') {
                // console.log(canvasChildren[i].attributes.x.value/20);
                // console.log(canvasChildren[i].attributes.y.value/20);
                thisDrawing.push(i - 2);

            }
        }
    }
    currentState.array.push(thisDrawing);
    console.log(currentState)
    localStorage.setItem('savedDrawing', JSON.stringify(currentState));
}

function showDrawings() {
    var canvas = Snap('#canvas');
    canvasChildren = canvas.node.children;
    savedDrawing = [];
    currentState = JSON.parse(localStorage.getItem('savedDrawing'));
    savedArea = document.getElementById('saved');
    savedArea.innerHTML = "";
    tempElements = []
    for (var i = 0; i < currentState.array.length; i++) {
        savedArea.appendChild(document.createElement('svg')).outerHTML = "<svg height='163' width='323' id='" + i + "'> </svg>";
        currentSVGSelector = document.getElementById(i);
        currentSVG = Snap(currentSVGSelector);
        currentPixels = currentState.array[i];
        console.log("svgnum" + i)
        console.log(currentSVG);
        listOfNewSquares = [];
        for (a = 0; a < 8; a++) {
            for (b = 0; b < 16; b++) {
                listOfNewSquares.push(currentSVG.rect(b * 20, a * 20, 20, 20));
                listOfNewSquares[(a * 16) + b].attr({ fill: 'white' })
                if (currentPixels.includes((a * 16) + b)) {
                    listOfNewSquares[(a * 16) + b].attr({ fill: 'blue' });
                }
                listOfNewSquares[(a * 16) + b].node.addEventListener("click", function (event) {
                    clearScreen();
                    console.log(event);
                    clickedSVG = event.target.parentNode;
                    children = clickedSVG.childNodes;
                    canvas = Snap('#canvas');
                    console.log(canvas.node.childNodes);
                    for (c = 0; c < canvas.node.childNodes.length + 1; c++) {
                        if (c > 2) {
                            if (clickedSVG.childNodes[c].attributes.fill.value === "#0000ff") {
                                canvas.node.childNodes[c - 1].attributes.fill.value = "#0000ff";
                                socket.emit('led', { coords: [{ x: canvas.node.childNodes[c - 1].attributes.x.value / 20, y: canvas.node.childNodes[c - 1].attributes.y.value / 20 }] });

                            }
                            console.log(canvas.node.childNodes[c].attributes.fill.value);
                        }
                    }
                })
            }
        }
        listOfSquare = [];
    }

    //     for (var y = 0; y < savedDrawings.length; y++) {
    //         savedDrawings[y].addEventListener('click', event => {
    //             copyingSnap = Snap(event.path[1]);
    //             console.log(copyingSnap);
    //             listOfSquare = [];
    //             for (a= 0; a < 8; a++) {
    //                 for (b = 0; b < 16; b++) {
    //                     listOfSquare.push(canvas.rect(b*20,a*20,20,20));
    //                     listOfSquare[(a*16)+b].click(clickCallback);
    //                     listOfSquare[(a*16)+b].attr({fill: 'white'});
    //             }

    //         }

    //     })
    // }


}

window.addEventListener("keydown", function (event) {
    if (event.defaultPrevented) {
        return; // Do nothing if the event was already processed
    }

    switch (event.key) {
        case "ArrowDown":
            // code for "down arrow" key press.
            if (sentDir != 2) {
                move('form', 2);
                sentDir = 2;
            }
            break;
        case "ArrowUp":
            // code for "up arrow" key press.
            if (sentDir != 1) {
                move('form', 1);
                sentDir = 1;
            }
            break;
        case "ArrowLeft":
            // code for "left arrow" key press.
            if (sentDir != 3) {
                move('form', 3);
                sentDir = 3;
            }
            break;
        case "ArrowRight":
            // code for "right arrow" key press.
            if (sentDir != 4) {
                move('form', 4);
                sentDir = 4;
            }
            break;
        case "-":
            document.getElementById("speed").value = parseInt(document.getElementById("speed").value,10) - 2;
            break;
        case "+":
            document.getElementById("speed").value = parseInt(document.getElementById("speed").value,10) + 2;
            break;
        default:
            return; // Quit when this doesn't handle the key event.
    }

    // Cancel the default action to avoid it being handled twice
    event.preventDefault();
}, true);

window.addEventListener("keyup", function (event) {
    if (event.defaultPrevented) {
        return; // Do nothing if the event was already processed
    }

    switch (event.key) {
        case "ArrowDown":
            // code for "down arrow" key press.
            if (sentDir === 2) {
                move(0, 2)
                sentDir = 0;
            }

        case "ArrowUp":
            // code for "up arrow" key press.
            if (sentDir === 1) {
                move(0, 1)
                sentDir = 0;
            }
        case "ArrowLeft":
            // code for "left arrow" key press.
            if (sentDir === 3) {
                move(0, 3)
                sentDir = 0;
            }
            break;
        case "ArrowRight":
            // code for "right arrow" key press.
            if (sentDir === 4) {
                move(0, 4)
                sentDir = 0;
            }
            break;
        default:
            return; // Quit when this doesn't handle the key event.
    }

    // Cancel the default action to avoid it being handled twice
    event.preventDefault();
}, true);
