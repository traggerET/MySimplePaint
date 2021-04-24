# MySimplePaint

Here is the final project of the second semester of my learning in MIPT. 

It's graphic editor. How I implemented it:

1)Using SFML, I encapsulated all the methods, objects and functions in class AdapterSFML so that 
if one decides to write it on other library, he needed only to rewrite AdapterSFML program without touching other parts of program.
2)The next step was to write basic classes to make event handling possible. It helps to create buttons, sliders, container menus etc and efficiently handle them.
3)THe third part was to create the editor itself i.e. Canvas, Palette, ToolMenu, Settings for tools(like thickness for brush) and Load/Save Image Buttons.
