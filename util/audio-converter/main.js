const fs = require( 'fs' );
const wavdecode = require( 'node-wav' ).decode;
const pcmconvert = require('pcm-convert');

let filename = process.argv[2];
if( !filename ){
	console.log( 'Please specify the path to a WAV file to decode.' );
	process.exit();
}


let buffer = fs.readFileSync( filename );
let result = wavdecode(buffer);


let converted = pcmconvert( result.channelData[0], 'float32', 'uint16' );

let output = 'int sample_wavetable_length = ' + result.channelData[0].length + ';\n';
output += 'const uint16_t sample_wavetable[' + result.channelData[0].length + '] = {\n';
output += '\t' + converted.join( ',' ) + '\n';
output += '};\n';

fs.writeFileSync( filename + '.txt', output );

console.log( 'DONE, results in ' + filename + '.txt - wavetable length: ', result.channelData[0].length );
