var parser = require('xml2json');
var fs = require('fs');

var myArgs = process.argv.slice(2);
var xml = fs.readFileSync(myArgs[0], 'utf8');
var svg = JSON.parse(parser.toJson(xml)).svg;
var pathInfo = svg.path.d;

var mEntries = pathInfo.split("M");
var lines = [];
mEntries.forEach( line  => {
    if (line.trim() != '') {
        lines.push(line.trim().replace('C', ' ')         // strip out 'C'
                                .replace('Z', ' ')       // strip out 'Z'
                                .replace(/\s\s+/g, ' ') // strip out double blanks
                              );
    }
});
var wallPlotterJson = {lines: []};
lines.forEach( line => {
    var points = {points: []};
    line.split(' ').forEach( coord => {
        var xy = coord.split(',');
        var point = {x: xy[0], y: xy[1]};
        if (point.x != '' && point.y != '') {
            points.points.push(point);
        }
    });
    wallPlotterJson.lines.push(points);
})
fs.writeFile('wall-plotter.json', wallPlotterJson, 'utf8', f => {
    console.log(JSON.stringify(wallPlotterJson));
});
