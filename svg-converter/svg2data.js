var parser = require('xml2json');
var fs = require('fs');
var Bezier = require('bezier-js');

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

if (pathInfo.includes('V') || pathInfo.includes('v') ||
    pathInfo.includes('L') || pathInfo.includes('l') ||
    pathInfo.includes('H') || pathInfo.includes('h') ||
    pathInfo.includes('m') || pathInfo.includes('c')) {
    console.log(pathInfo);
    console.log("Not supported SVG-path directive.");
    return;
}

var mEntries = pathInfo.split("M");
var lines = [];
mEntries.forEach( line  => {
    if (line.trim() != '') {
        lines.push(line.trim().replace('C', '')
            .replace('Z', '')
            .replace(/\s\s+/g, ' '));
    }
});
var wallPlotterJson = {lines: []};
var zeroPoint = {x: 0,y: 0};
var firstPoint = {x:0,y:0};

lines.forEach( line => {
    var points = {points: []};
    console.log(line);
    line.split(' ').forEach( coord => {
        var xy = coord.split(',');
        var point = {x: parseFloat(xy[0]),
            y: parseFloat(xy[1])};

        if (!isNaN(point.x) && !isNaN(point.y))  {
            point.x = parseFloat(xy[0]);
            point.y = parseFloat(xy[1]);
            /*            if (points.points.length === 1) {
                            console.log("FIRSTPOINT")
                            console.log("X:" + xy[0]);
                            console.log("Y:" + xy[1]);
                            firstPoint.x = parseFloat(xy[0]) - parseFloat(firstPoint.x);
                            firstPoint.y = parseFloat(xy[1]) - parseFloat(firstPoint.y);
                            console.log(firstPoint);
                        }
                        lastPoint.x = parseFloat(xy[0]);
                        lastPoint.y = parseFloat(xy[1]);
            */            points.points.push(point);
        }
    });
//    points.points.push(zeroPoint);
    wallPlotterJson.lines.push(points);
})

// bezier
var newWallPlotterJson = {lines: []};

var getBezierPoints = function(start, mid, end) {
    var curve = new Bezier(start, mid, end);
    return curve.getLUT(5);
};

var bezierCoords = [{x:0, y:0},{x:0, y:0},{x:0, y:0}];

wallPlotterJson.lines.forEach(line => {
    var counter = 0;
    var points = {points: []};
    line.points.forEach(point => {
        if (counter > 0) {
//            console.log(bezierCoords);
            bezierCoords[counter - 1].x = point.x;
            bezierCoords[counter - 1].y = point.y;
        } else {
            points.points.push(point);
            console.log("M:");
            console.log(point);
        }
        counter++;
        if (counter == 4) {
            counter = 0;
            var bezierPoints = getBezierPoints(bezierCoords[0], bezierCoords[1], bezierCoords[2]);
            console.log(bezierPoints);
            bezierPoints.forEach( bPoint => {
                points.points.push(bPoint);
            })
        }
        /*        if (counter > 3) {
                    points.points.push(point);
                }
        */    });
    newWallPlotterJson.lines.push(points);
});
wallPlotterJson = newWallPlotterJson;

//relative path
var newWallPlotterJson = {lines: []};
var lastPoint = {x: 0,y: 0};
var zeroPoint = {x: 0,y: 0};
var firstPoint = {x:0,y:0};

wallPlotterJson.lines.forEach( line => {
    var points = {points: []};
    line.points.forEach(point => {
        if (!isNaN(point.x) && !isNaN(point.y)) {
            var x = (point.x - parseFloat(lastPoint.x)) * parseFloat(zoom);
            var y = (point.y - parseFloat(lastPoint.y)) * parseFloat(zoom);
            /*            if (points.points.length === 1) {
                            console.log("FIRSTPOINT")
                            console.log(firstPoint);
                            firstPoint.x = parseFloat(xy[0]) - parseFloat(firstPoint.x);
                            firstPoint.y = parseFloat(xy[1]) - parseFloat(firstPoint.y);
                            console.log(firstPoint);
                        }
            */            lastPoint.x = point.x;
            lastPoint.y = point.y;
            point.x = x;
            point.y = y;
            points.points.push(point);
        }
    });
    points.points.push(zeroPoint);
    newWallPlotterJson.lines.push(points);
});

// data
var data = "";
wallPlotterJson.lines.forEach(line => {
    data += "m\n";
    line.points.forEach(point => {
        data += point.x + "," + point.y + "\n";
    });
});

fs.writeFile('wall-plotter.data', data, 'utf8', f => {
//    console.log(data);
});

// back to svg
var path = "";
wallPlotterJson.lines.forEach(line => {
    path += "\nm ";
    line.points.forEach(point => {
        path += point.x + "," + point.y + " ";
    });
});

if (pathGroup != undefined) {
    svg.svg.path = {};
    svg.svg.g = undefined;
}
svg.svg.path.d = path;
xml = parser.toXml(svg);

fs.writeFile('wall-plotter.svg', xml, 'utf8', f => {
//    console.log(xml)
});
