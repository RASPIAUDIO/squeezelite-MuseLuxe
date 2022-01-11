<h2 align="center">Boilerplate - Bootstrap v4 - SASS - JQuery - WebPack</h2>

<p align="center">    
            <a href="https://webpack.js.org/"><img alt="Webpack" src="https://img.shields.io/badge/Webpack-4.41.6-%238DD6F9.svg"></a>
                <a href="https://babeljs.io/"><img alt="Webpack" src="https://img.shields.io/badge/Babel%2FCore-7.8.4-%23f5da55.svg"></a>
                <a href="https://www.npmjs.com/package/gulp-sass"><img alt="node-sass" src="https://img.shields.io/badge/node--sass-v4.13.1-ff69b4.svg"></a>
                <a href="https://jquery.com/"><img src="https://img.shields.io/badge/jQuery-3.3.1-blue.svg" alt="jquery"></a>
                <a href="https://lodash.com/"><img src="https://img.shields.io/badge/lodash-4.17.15-blue.svg" alt="jquery"></a>
                <a href="https://popper.js.org/"><img src="https://img.shields.io/badge/popper.js-2.0.6-blue.svg" alt="popper.js"></a>
                <a href="https://eslint.org/"><img src="https://img.shields.io/badge/es--lint-5.15.1-%23463fd4.svg" alt="eslint"></a>
                <a href="https://fontawesome.com/"><img alt="Font Awesome" src="https://img.shields.io/badge/Font--Awesome-5.12.1-blue.svg"></a>
                <a href="https://icons8.com/line-awesome"><img alt="Line Awesome" src="https://img.shields.io/badge/Line%20Awesome-1.3.0-green"></a>
</p>

![webpack logo](https://abload.de/img/webpack1tkeb.png)
![bootstrap logo](https://abload.de/img/bootstrap-logo-vector78khf.png)
![babel logo](https://abload.de/img/2000px-babel_logo.svgrzkxw.png)
![sass logo](https://abload.de/img/1280px-sass_logo_colo0bjb4.png)


<p align="center">
  <em>
  SASS
  · Babel
  · Bootstrap
  · JQuery
  · PopperJS
  · Font Awesome
  </em>
</p>

This Webpack4-Sass Boilerplate contains the following features:

- Webpack4 & Dev-Server
- TypeScript 3.7.5
- Babel ES6 Compiler
- Bootstrap v4 - with Theme Support
- Font Awesome v5.7
- Animate.css Library v3.7.2
- JQuery v3.3.1
- PopperJS v2
- _lodash
- concentrate and minify JavaScript.
- Compile, minify, Autoprefix SASS.
- Optimize and Cache Images
- Preconfigured BootsWatch Template (YETI & Slate)
- Linting for your TS, JS and SASS

## Features

### Webpack Loaders & Plugins

This project contains the following loaders & plugins:

- `node-sass` for compiling sass (SCSS)
- `babel-loader` for compiling ES6 code
- `babel-eslint && eslint-loader` for Linting your .js
- `tslint` for Linting your .ts 
- `lodash-webpack-plugin` create smaller Lodash builds by replacing feature sets of modules with noop, identity, or simpler alternatives.
- `webpack-dev-server` for serving & Hot-Reloading
- `css-loader` for compressing css
- `sass-loader` for compressing and loading scss & sass
- `url- & file-loader` for loading and optimizing images
- `xml and csv loader` for loading data files
- `html-loader` for loading & optimizing html files
- `clean-webpack-plugin` for keeping your dist folder clean
- `favicons-webpack-plugin` generate favicons form your "logo.png"


## Getting Started

### Dependencies

Make sure these are installed first.

- [Node.js](http://nodejs.org)
- [Webpack](https://webpack.js.org/guides/installation/)

     `npm install --g webpack`

<hr/>

### Quick Start

1. Clone the repo :
      `git clone https://github.com/AndyKorek/webpack-boilerplate-sass-ts-bootstrap4-fontawesome.git`
2. In bash/terminal/command line, `cd ` into project directory.
3. Run `npm i` to install required dependencies.

4. Run the Dev Server with (with Hot Reloading) `npm run dev`

<hr/>

### Build the Production Folder
`npm run build`

This will:

- Bundle and Minify SASS(scss) to css & Hash and Cash it
- generate GZip and Brodli Compressed Assets
- Bundle and Minify JS
- Optimize Images
- Optimize HTML
- generate Favicons

<hr/>

## Documentation

### Workflow structure

`src` - > source directory

`dist` -> build directory


```

├── src
│   ├── assets
│   │   └── images
│   ├── fonts
│   ├── sass
│   │   ├── layout
|   |   |     └── _features.scss
│   │   ├── setup
|   |   |     └── _normalize.scss
│   │   ├── themes
|   |   |     ├── _slate.scss
|   |   |     └── _yeti.scss
│   │   ├── utils
|   |   |     ├── _mixins.scss
|   |   |     └── _variables.scss
│   │   ├── _globals.scss
│   │   ├── _headings.scss
│   │   ├── _typography.scss
│   │   ├── _vendor.scss
│   │   └── main.scss
│   ├── ts
│   │   ├── custom.ts
│   │   ├── line-awesome.ts
│   │   ├── vendor.ts
│   |── .htaccess
│   |── 404.html
│   |── index.html
│   └── index.ts



├── dist
│   ├── assets
│   │   ├── images
│   │   └── 
│   ├── css
│   │   ├── vendors.[contenthash].css
│   │   └── main.contenthash].css
│   ├── js
│   │   ├── main.[contenthash].js
│   │   ├── runtime.[contenthash].js
│   │   └── vendors.[contenthash].js
│   │   
│   └── index.html

```
### Loading the Features you need

in  `src/js/vendor/_boostrap.js` uncomment all Features you need

put your custom js to `src/js/_custom.js`


<hr/>

### Instructions

- Add `sass`(.scss) files to `src/_scss` folder.

    - Make sure you import the scss file in `main.scss`
      ```
      @import "filename";
      ```
- Add your assets to `src/assets/`

- Add `images` to `src/assets/images`

## TODO list

- [x] Bootstrap 4
- [x] Webpack 4
- [x] Jquery
- [x] PopperJS v2
- [x] Include ES-Lint
- [x] Font-Awesome
- [x] Assets Loader
- [x] Separated location for Bundled Files
- [x] Adding EsLint
- [ ] Code Optimising
- [x] Uglify and Minify JS with Terser

## Licence

Code released under the [MIT License](https://github.com/AndyKorek/webpack4_boilerplate/blob/master/LICENSE).

*</> with* :heart: *from Germany*
