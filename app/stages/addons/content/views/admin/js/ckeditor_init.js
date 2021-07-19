var config = {
    toolbar:
    [
        [ 'Source','-','DocProps','Preview','Print','-','Maximize', 'ShowBlocks'],
        [ 'Cut','Copy','Paste','PasteText','PasteFromWord','-','Undo','Redo' ],
        [ 'Bold','Italic','Underline','Strike','Subscript','Superscript','-','RemoveFormat' ],
        [ 'Image','Flash','Table','HorizontalRule','PageBreak','-', 'TextColor','BGColor' ],
        [ 'NumberedList','BulletedList','-','Outdent','Indent','-','Blockquote','CreateDiv','-','JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock','-','BidiLtr','BidiRtl','-','Link','Unlink','-','Styles','Format','Font','FontSize' ],
        
    ],
    removePlugins : 'resize',
    extraPlugins : 'autogrow',
    autoGrow_maxHeight :450,
    language:'en',
    toolbarStartupExpanded: true
};
$.fn.ckeditor && $('textarea.edit').ckeditor(config);