var parser = require('xml2json');
var fs = require('fs');
const { pathDataToPolys } = require('./svg-path-to-polygon.js');


var myArgs = process.argv.slice(2);
var xml = fs.readFileSync(myArgs[0], 'utf8');
var zoom  = myArgs[1];
if (isNaN(zoom)) {
    zoom = 1;
}
console.log("Zoom: " + zoom);

var svg = JSON.parse(parser.toJson(xml));
var pathInfo = '';
var pathGroup = svg.svg.g;
if (pathGroup != undefined) {
    pathGroup.path.forEach(path => {
        pathInfo += path.d;
    })
} else {
    pathInfo = svg.svg.path.d;
}



// returns a window with a document and an svg root node
const window = require('svgdom');
const document = window.document;
const {SVG, registerWindow} = require('@svgdotjs/svg.js');

// register window and document
registerWindow(window , document);

// create canvas
const canvas = SVG(document.documentElement);

// use svg.js as normal
canvas.path(pathInfo);

// get your svg as string
console.log(canvas.svg());

// or
//console.log(canvas.node.outerHTML);
//console.log(canvas.node);

var testsvg = SVG(svg);
console.log(testsvg);
console.log(testsvg.node.svg.path.d);
//testsvg.size(testsvg.size / 10);


let path2polyPoints = pathDataToPolys(pathInfo, {tolerance:1, decimals:1});
console.log(path2polyPoints);
//relative
var lines = [];
var point = {x: 0,y: 0};
var lastPoint = {x: 0,y: 0};
path2polyPoints.forEach( line  => {
    var points = [];
    line.forEach(point => {
        point.x = (parseFloat(point[0]) - lastPoint.x) * zoom;
        point.y = (parseFloat(point[1]) - lastPoint.y) * zoom;
        lastPoint.x = parseFloat(point[0]);
        lastPoint.y = parseFloat(point[1]);
        points.push(point);
    });
    lines.push(points);
});
//console.log(lines);

var newPath = "";
lines.forEach( line => {
    newPath += "m\n";
    line.forEach(point => {
        newPath += point.x + "," + point.y + "\n";
    });
})
fs.writeFile('wall-plotter.data', newPath, 'utf8', f => {
//    console.log(newPath);
});

svg.svg.path.d = newPath;
xml = parser.toXml(svg);

fs.writeFile('wall-plotter.svg', xml, 'utf8', f => {
//    console.log(xml)
});
