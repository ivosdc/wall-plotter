# wall-plotter cli svg-converter

- npm install


# about this tool
*This Version:*
extracts the first svg:path the :d information and converts *"M x y"* (SVGs/G-code uppercase M paramater) to the wall-plotter json format:
```
<svg xmlns="http://www.w3.org/2000/svg"
     width="8.88889in" height="5.55556in"
     viewBox="0 0 640 400">
  <path id="Pfad"
        fill="none" stroke="black" stroke-width="1"
        d="M 148.69,233.00 157.00,211.00 153.78,211.00 147.02,228.97 140.22,211.00 137.00,211.00 145.33,233.00 148.69,233.00
	   M 164.55,233.00 168.30,233.00 173.02,213.91 177.69,233.00 181.44,233.00 187.00,211.00 183.91,211.00 179.05,230.19
             174.36,211.00 171.67,211.00 166.94,230.19 162.09,211.00 159.00,211.00 164.55,233.00" />
</svg>
```
``` 
{lines: [
    {points: [
        {x: 158.21149, y:280.34488},
        {x:3.94531,y:0},
        {x:0,y:11.95312},
        {x:-3.94531,y:0},
        {x:0,y:-29.16016}]},
     {points: [
        {x:212.74274, y:279.11441},
        {x:3.59375,y:0},{x:0,y:30.39063},
        {x:-3.94531,y:0},
        {x:0,y:-29.16016}
        ]
    }
]}";
```
Every section "M" of a svg/xml-path section will be converted into a `points-array` with `X` and `Y` coords.
By the way, the wall-plotter json format corresponds to the svg/G-code *small "m"* parameter.
Big "M" means absolute X,Y values. Small "m" means the next point X,Y are the diff to the last X,Y.

## Usage
```
> node svg2json.js example.svg
```
The json-output will be stored as `wall-plotter.json`.


# how to create a "working" SVG

I'm using `gimp` to create the svg templates. Gimp creates path-instructions in a single path-tag with multiple (big) "M" lines.
Just plain extraction of the `svg:path:-d` information is done.
The parser crawls and splits "M" parts in the path.

## In gimp you can convert drawings to path instructions:
- chose select from alpha (e.g.) (or your prefered selection)
- convert selection into path
- export paths-layer as svg

***thats all***

These SVG-files can easily be converted to wall-plotter.json!