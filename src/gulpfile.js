var minify = require('gulp-minifier');
var gulp = require('gulp');
var del = require('del');

gulp.task('clean', function(){
  return del(['../data/*'],{force:true});
});
 
gulp.task('default', gulp.series('clean', function() {
  return gulp.src('./html/**/*').pipe(minify({
    ext: {
      min: '.min.js'
    },
    ignoreFiles: ['-min.js','gulpfile.js'],
    minify: true,
    minifyHTML: {
      collapseWhitespace: true,
      conservativeCollapse: true,
    },
    minifyJS: {
      sourceMap: true
    },
    minifyCSS: true,
    getKeptComment: function (content, filePath) {
        var m = content.match(/\/\*![\s\S]*?\*\//img);
        return m && m.join('\n') + '\n' || '';
    }
  })).pipe(gulp.dest('../data'));
}));
