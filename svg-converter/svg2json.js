var parser = require('xml2json');
var fs = require('fs');

var myArgs = process.argv.slice(2);
var svgFile = myArgs[0];

var xml = fs.readFileSync(svgFile, 'utf8');

console.log("input -> %s", xml)

// xml to json
var json = parser.toJson(xml);
console.log("to json -> %s", json);

// json to xml
var xml = parser.toXml(json);
console.log("back to xml -> %s", xml)