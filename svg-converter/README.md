# wall-plotter cli svg-converter

- npm install


# about this tool
*This Version:*
extracts the first svg:path the :d information and converts *"M x y"* (SVGs/G-code uppercase M paramater) to the wall-plotter.data format:
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
m
-9.55,16.24
-7.00,-1.00
7.00,1.00
m
43.00,0.00
-14.00,-1.00
14.00,1.00
m
-56.00,1.00
-10.00,-1.00
10.00,1.00
m
53.00,0.96
11.00,-0.96
```
Every section "M" of a svg/xml-path section will be converted into a `points-array` with `X` and `Y` coords.
By the way, the *wall-plotter.data* format corresponds to the svg/G-code *small "m"* parameter.
Big "M" means absolute X,Y values. Small "m" means the next point X,Y are the diff to the last X,Y.

I found a good explanation of the svg-path:  https://css-tricks.com/svg-path-syntax-illustrated-guide/ .


We currently struggle with the big `"M"` only!

L, l, H, h, V, v and  m have to be done... Hands on! Contributers welcome ;)

# how to create a "working" SVG

I'm using `gimp` to create the svg templates. Gimp creates path-instructions in a single path-tag with multiple (big) "M" lines.
Just plain extraction of the `svg:path:-d` information is done.
The parser crawls and splits "M" parts in the path.

## In gimp you can convert drawings to path instructions:
I prefer to:
- Set the picture mode to b/w and back to rgb. Rgb is needed for the next step.
- Use `color to transparence`. Set your background color to transparent.
- Right-click the layer. Choose select from alpha.
- Convert selection into path (`selection` -> `to path`)
- Switch from *layers* to *path*
- Right-click the path-layer and choose export, use .svg as suffix. This exports the path-layer as svg.

***thats all***

These SVG-files can easily be converted to wall-plotter.data!

## Usage
```
> node svg2json.js example.svg
```
The data-output will be stored as `wall-plotter.data`.
