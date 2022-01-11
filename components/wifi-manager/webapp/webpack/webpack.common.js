/* eslint-disable  */
// Common Config is used in Development and Production Mode.
const path = require('path');
const CleanWebpackPlugin = require('clean-webpack-plugin');
const webpack = require('webpack');
const HtmlWebPackPlugin = require('html-webpack-plugin');
const LodashModuleReplacementPlugin = require('lodash-webpack-plugin');
const ScriptExtHtmlWebpackPlugin = require('script-ext-html-webpack-plugin');
const StylelintPlugin = require('stylelint-webpack-plugin');
const ESLintPlugin = require('eslint-webpack-plugin');
const SpriteLoaderPlugin = require('svg-sprite-loader/plugin');
// Linting
const TSLintPlugin = require('tslint-webpack-plugin');
const ImageminPlugin = require('imagemin-webpack-plugin').default;
const imageminMozjpeg = require('imagemin-mozjpeg');


module.exports = {
    entry: {
        index: './src/index.ts'
    },
    output: {
        path: path.resolve(__dirname, 'dist'),
        filename: './js/[name].[hash:6].bundle.js'
    },
    module: {
        rules: [
            // Raw Loader
            {
                test: /\.txt$/,
                use: 'raw-loader'
            },
            //  HTML Loader
            {
                test: /\.html$/,
                use: [
                    {
                        loader: 'html-loader',
                        options: {minimize: true}
                    }
                ]
            },
            //  CSS/SCSS Loader & Minimizer
            {
                test: /\.(sa|sc|c)ss$/,
                use: [
                  "style-loader",
                  "css-loader",
                    {
                        loader: 'postcss-loader',
                        options: {
                          postcssOptions: {
                            parser: "sugarss",
                          },
                      },
                    },
                    {
                        loader: 'resolve-url-loader',
                        options: {}
                    },
                    {
                        loader: 'sass-loader',
                        options: {
                            sourceMap: true,
                            sourceMapContents: false

                        }
                    }
                ],
                
            },
            {
              test: /\.svg$/,
              use: [
                { 
                  loader: 'svg-sprite-loader',
                  options: { 
                    extract: true,
                } },
                'svg-transform-loader',
                {
                  loader: 'svgo-loader',
                  options: {
                    plugins: [
                      {removeTitle: true},
                      {convertColors: {shorthex: false}},
                      {convertPathData: false},
                      {convertPathData:true}
                    ]
                  }
                }
              ]
            },
            // Image Loader
            {
                test: /\.(png|jpeg|jpg|webp|gif|ico)/i,
                use: [
                    {
                        loader: 'url-loader',
                        options: {
                           // publicPath: '../',
                            //name: './assets/images/' + '[name].[ext]',
                            limit: 10000,
                            //limit:false,
                            //publicPath: '../'
                        }

                    },
                ]
            },
            // Babel Loader
            {
                test: /\.ts(x?)$/,
                exclude: /node_modules/,
                loader: 'babel-loader'
            },
            {
                test: /\.m?js$/,
                exclude: /(node_modules|bower_components)/,
                use: {
                  loader: 'babel-loader',
                  options: {
                    presets: ['@babel/preset-env'],
                    plugins: [
                        '@babel/plugin-proposal-object-rest-spread',    
                        '@babel/plugin-proposal-nullish-coalescing-operator',
                        '@babel/plugin-proposal-optional-chaining',
                        '@babel/plugin-proposal-class-properties'
                    ]
                  }
                },
            },
            // XML Loader
            {
                test: /\.xml$/,
                use: [
                    'xml-loader'
                ]
            }, 
            {
                test: require.resolve("bootstrap"),
                loader: "expose-loader",
                options: {
                  exposes: ["bootstrap"],
                },
              },          
              {
                test: require.resolve("jquery"),
                loader: "expose-loader",
                options: {
                  exposes: ["$", "jQuery"],
                },
              },              
              {
                test: require.resolve("underscore"),
                loader: "expose-loader",
                options: {
                  exposes: [
                    "_.map|map",
                    {
                      globalName: "_.reduce",
                      moduleLocalName: "reduce",
                    },
                    {
                      globalName: ["_", "filter"],
                      moduleLocalName: "filter",
                    },
                  ],
                },
              },
            
        ]
    },
    resolve: {
                 extensions: ['.js', '.jsx', '.tsx', '.ts', '.json'],
                 alias: {
                  riSvg: 'remixicon/icons/'
                }
    },

    plugins: [
      new CleanWebpackPlugin(),
      new ImageminPlugin({
          test: /\.(jpe?g|png|gif|svg)$/i,
          // lossLess gif compressor
          gifsicle: {
              optimizationLevel: 9
          },
          // lossy png compressor, remove for default lossLess
          pngquant: ({
              quality: '75'
          }),
          // lossy jpg compressor
          plugins: [imageminMozjpeg({
              quality: '75'
          })],
          destination: './webpack',
      }),       
        new ESLintPlugin({
            cache: true,
            ignore: true,
            useEslintrc: true,
          }),
        new HtmlWebPackPlugin({
            title: 'SqueezeESP32',
            template: './src/index.ejs',
            filename: 'index.html',
            inject: 'body',
            minify: {
              html5                          : true,
              collapseWhitespace             : true,
              minifyCSS                      : true,
              minifyJS                       : true,
              minifyURLs                     : false,
              removeAttributeQuotes          : true,
              removeComments                 : true, // false for Vue SSR to find app placeholder
              removeEmptyAttributes          : true,
              removeOptionalTags             : true,
              removeRedundantAttributes      : true,
              removeScriptTypeAttributes     : true,
              removeStyleLinkTypeAttributese : true,
              useShortDoctype                : true
            },
            favicon: "./src/assets/images/favicon-32x32.png",
            
            excludeChunks: ['test'],
        }),
        
        new ScriptExtHtmlWebpackPlugin({
            defaultAttribute: 'defer'
        }),

        // // Load Lodash Features Separately https://www.npmjs.com/package/lodash-webpack-plugin
        new LodashModuleReplacementPlugin({
            'collections': true,
            'paths': true,
        }),
        new TSLintPlugin({
          files: ['./src/ts/*.ts']
      }),        
         new StylelintPlugin( {
            files: ['./src/sass/*.s?(a|c)ss'],
            configFile: './config/.stylelintrc',
            emitError: true,
            emitWarning: true,
            failOnError: false,
            fix: true
        }),
        new SpriteLoaderPlugin({plainSprite: true})
    ],
};
