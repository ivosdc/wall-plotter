var parser = require('xml2json');
var fs = require('fs');

var myArgs = process.argv.slice(2);
var xml = fs.readFileSync(myArgs[0], 'utf8');
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
    pathInfo.includes('m')) {
    console.log(pathInfo);
    console.log("Not supported SVG-path directive.....");
    return;
}


var mEntries = pathInfo.split("M");
var lines = [];
mEntries.forEach( line  => {
    if (line.trim() != '') {
        lines.push(line.trim().replace('C', '').replace('c', '')
            .replace('Z', '')
            .replace(/\s\s+/g, ' '));
    }
});
var wallPlotterJson = {lines: []};
var lastPoint = {x: 0,y: 0};
var zeroPoint = {x: 0,y: 0};

lines.forEach( line => {
    var points = {points: []};
    line.split(' ').forEach( coord => {
        var xy = coord.split(',');
        var point = {x: parseFloat(xy[0]),
                    y: parseFloat(xy[1])};

        if (!isNaN(point.x) && !isNaN(point.y))  {
            point.x = parseFloat(xy[0]) - parseFloat(lastPoint.x);
            point.y = parseFloat(xy[1]) - parseFloat(lastPoint.y);
            lastPoint.x = parseFloat(xy[0]);
            lastPoint.y = parseFloat(xy[1]);
            points.points.push(point);
        }
    });
    points.points.push(zeroPoint);
    wallPlotterJson.lines.push(points);
})

//collect small values and add them to a valid "point"-value
var filteredPlotData = {lines: []};
var plotDataString = "";
var smallPoint = {x: 0,y: 0};
wallPlotterJson.lines.forEach(line => {
    var points = {points: []};
    line.points.forEach(point => {
       point.x = parseFloat(point.x);
       point.y = parseFloat(point.y);
       if (Math.abs(point.x) < 0.5 && Math.abs(point.y) < 0.5) {
           smallPoint.x += point.x;
           smallPoint.y += point.y;
       }
       if ((Math.abs(point.x) >= 0.5) || (Math.abs(point.y) >= 0.5)) {
           point.x += parseFloat(smallPoint.x);
           point.y += parseFloat(smallPoint.y);
           point.x = (point.x).toFixed(2);
           point.y = (point.y).toFixed(2);
           points.points.push(point);
           smallPoint.x = 0;
           smallPoint.y = 0;
       }
       if (Math.abs(parseInt(smallPoint.x)) >= 0.5 || Math.abs(parseInt(smallPoint.y)) >= 0.5) {
           smallPoint.x = (smallPoint.x).toFixed(2);
           smallPoint.y = (smallPoint.y).toFixed(2);
           points.points.push(smallPoint);
           smallPoint.x = 0;
           smallPoint.y = 0;
       }
    });
    filteredPlotData.lines.push(points);
});
wallPlotterJson = filteredPlotData;

// data
var data = "";
wallPlotterJson.lines.forEach(line => {
    data += "m\n";
    line.points.forEach(point => {
        data += point.x + "," + point.y + "\n";
    });
});

fs.writeFile('wall-plotter.data', data, 'utf8', f => {
    console.log(data);
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
    console.log(xml)
});

