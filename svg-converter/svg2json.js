var parser = require('xml2json');
var fs = require('fs');

var myArgs = process.argv.slice(2);
var xml = fs.readFileSync(myArgs[0], 'utf8');
var svg = JSON.parse(parser.toJson(xml));
var pathInfo = svg.svg.path.d;

var mEntries = pathInfo.split("M");
var lines = [];
mEntries.forEach( line  => {
    if (line.trim() != '') {
        lines.push(line.trim().replace(/\s\s+/g, ' ')); // strip out double blanks
    }
});
var wallPlotterJson = {lines: []};
var lastPoint = {x: 0,y: 0};
lines.forEach( line => {
    var points = {points: []};
    line.split(' ').forEach( coord => {
        var xy = coord.split(',');
        var point = {x: parseFloat(xy[0]),
                    y: parseFloat(xy[1])};

        if (!isNaN(point.x) && !isNaN(point.y))  {
            point.x = parseFloat(xy[0]) - lastPoint.x;
            point.y = parseFloat(xy[1]) - lastPoint.y;
            lastPoint.x = parseFloat(xy[0]);
            lastPoint.y = parseFloat(xy[1]);
            points.points.push(point);
        }
    });
    wallPlotterJson.lines.push(points);
})
wallPlotterJson.lines.forEach(line => {
   line.points.forEach(point => {
       point.x = parseFloat(point.x).toFixed(2);
       point.y = parseFloat(point.y).toFixed(2);
   });
});

fs.writeFile('wall-plotter.json', JSON.stringify(wallPlotterJson), 'utf8', f => {
    console.log(JSON.stringify(wallPlotterJson));
});

// back to svg
var path = "";
wallPlotterJson.lines.forEach(line => {
    path += "\nm ";
    line.points.forEach(point => {
        path += point.x + "," + point.y + " ";
    });
});
svg.svg.path.d = path;
xml = parser.toXml(svg);

fs.writeFile('wall-plotter.svg', xml, 'utf8', f => {
    console.log(xml)
});

