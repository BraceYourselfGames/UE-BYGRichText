# BYG Rich Text Library

![Example Output](https://benui.ca/assets/unreal/bygrichtext-output.png)

This plugin for Unreal Engine 4 is an alternative to UE4's built in rich text
system.

It differs from Unreal's default rich text implementation in a few ways:

* Support for Markdown and customizable markup.
* Support for nested styles.
* Simple to extend supported text properties in C++.

## Feature Comparison

| Feature | Unreal Rich Text | BYG Rich Text |
| --- | --- | --- |
| Nested styles					| :x:					| :heavy_check_mark:	|
| Customizable syntax   		| :x:					| :heavy_check_mark:	|
| Markdown-like shortcuts		| :x:					| :heavy_check_mark:	|
| Inline images					| :heavy_check_mark:	| :heavy_check_mark:	|
| Style-based justification		| :x: (block only)		| :heavy_check_mark:	|
| Style-base margins			| :x: (block only)		| :heavy_check_mark:	|
| Inline tooltips				| :heavy_check_mark:	| :heavy_check_mark:	|
| Customizable paragraph separator | :x: | :heavy_check_mark:					|
| XML-like syntax   			| :heavy_check_mark:	| :heavy_check_mark:	|
| Datatable-based stylesheet	| :heavy_check_mark:	| :x:					|
| Blueprint code support		| :heavy_check_mark:	| :x:					|


## Usage

### Creating a custom stylesheet

1. Create a new Blueprint asset, with `BYGRichTextStylesheet` as the`parent
   class.
2. Add styles to the stylesheet.
3. Add properties to each style.

* Each stylesheet can have one or more _Styles_.
* Each style can hae one or more _Properties_ applied to it. 

#### Rich Text Properties

* **Font/Typeface**
* **Text style:** bold, italic and other styles.
* **Text size**
* **Text color**
* **Text case:** force uppercase or lowercase
* **Text shadow (color and distance)**
* **Margin:** Add spacing between paragraphs.
* **Justification:** Align text left, right or center.
* **Line Height:** Change the spacing between lines.
* **Text Wrap** 
* **Background:** Set a background color or image.

### Using the Rich Text Block

1. Create a UserWidget blueprint.
2. From the widget palette, under `Brace Yourself Games`, find `BYG Rich Text Block` and add it to your UserWidget.
3. By default, the stylesheet selected will be the Markdown example included with the project.
4. Try entering `Hello *bold world*, hello _italic world_!`

![Example Stylesheet](https://benui.ca/assets/unreal/bygrichtext-stylesheet.png)

## Installation

### Source

1. Download the zip or clone the repository to `ProjectName/Plugins/BYGRichText`.
2. Add `BYGRichText` to `PrivateDependencyModuleNames` inside `ProjectName.Build.cs`.

## Unreal Version Support

* Unreal Engine 4.26
* May work with previous versions, but not tested

## License

* [3-clause BSD license](LICENSE)

## Contact

* Created and maintained by [@_benui](https://twitter.com/_benui) at [Brace Yourself Games](https://braceyourselfgames.com/)


## FAQ

### Q) How do I change the default Stylesheet used on new BYG Rich Text widgets?

**A)** Open **Project Settings**, then under **Plugins**, select **BYG Rich Text**. Change the
Default Stylesheet property to your desired Stylesheet.

### Q) How do I change the default appearance of my text?

**A)** By default, text without a specified tag will be styled based on the
**Default Style Name** property within a stylesheet.
Create a style in your stylesheet named `default` and customize its properties
to change the appearance of un-tagged text.

### Q) Can I use this in a commercial game?

**A)** Yes, the license is the [3-clause BSD license](LICENSE) which means it can be used in commercial works so long as the copyright notice is included.

### Q) Has this been used in any games?

**A)** This is based on the rich text system implemented for [Industries of
Titan](https://braceyourselfgames.com/industries-of-titan/)

### Q) Is stylesheet inheritance supported?

**A)** Currently it is not supported. I am looking into it.

### Q) My text is pink, what's going wrong?

**A)** Make sure that you have selected a **Stylesheet Class** from within the
**BYG Rich Text Block properties**.
