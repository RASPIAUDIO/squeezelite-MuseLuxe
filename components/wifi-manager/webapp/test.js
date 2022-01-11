const stats='';
// Merges webpack.common config with this production config
const fs = require('fs');
const glob = require('glob');
var getDirectories = function (src, callback) {
glob(src + '/**/*.gz', callback);
};
getDirectories('./webpack/', function (err, list) {
if (err) {
    console.log('Error', err);
} else {
    const regex = /^(.*\/)([^\/]*)$/
    const relativeRegex = /(\w+\/[^\/]*)$/
    const makePathRegex = /([^\.].*)$/
    let exportDefHead=
    '/***********************************\n'+
    'webpack_headers\n'+
    stats+'\n'+
    '***********************************/\n'+
    '#pragma once\n'+
    '#include <inttypes.h>\n'+
    'extern const char * resource_lookups[];\n'+
    'extern const uint8_t * resource_map_start[];\n'+
    'extern const uint8_t * resource_map_end[];\n';
    let exportDef=  '// Automatically generated. Do not edit manually!.\n'+
                    '#include <inttypes.h>\n';
    let lookupDef='const char * resource_lookups[] = {\n';
    let lookupMapStart='const uint8_t * resource_map_start[] = {\n';
    let lookupMapEnd='const uint8_t * resource_map_end[] = {\n';
    let cMake='';
    list.forEach(fileName=>{
        console.log(fileName);
            let exportName=fileName.match(regex)[2].replace(/[\. \-]/gm,'_');
            let relativeName=fileName.match(relativeRegex)[1];
            exportDef+=	'extern const uint8_t '+exportName+'_start[] asm("_binary_'+exportName+'_start");\n'+
                    'extern const uint8_t '+exportName+'_end[] asm("_binary_'+exportName+'_end");\n';
            lookupDef+='\t"/'+relativeName+'",\n';
            lookupMapStart+='\t'+ exportName+'_start,\n';
            lookupMapEnd+= '\t'+ exportName+'_end,\n';
            cMake+='target_add_binary_data( __idf_wifi-manager ./webapp'+fileName.match(makePathRegex)[1]+' BINARY)\n';
            
    });
    lookupDef+='""\n};\n';
    lookupMapStart=lookupMapStart.substring(0,lookupMapStart.length-2)+'\n};\n';
    lookupMapEnd=lookupMapEnd.substring(0,lookupMapEnd.length-2)+'\n};\n';
    try {
        fs.writeFileSync('webapp.cmake', cMake);
        fs.writeFileSync('webpack.c', exportDef+lookupDef+lookupMapStart+lookupMapEnd);
        fs.writeFileSync('webpack.h', exportDefHead);
        //file written successfully
        } catch (err) {
        console.error(err);
        }        
}
});                
