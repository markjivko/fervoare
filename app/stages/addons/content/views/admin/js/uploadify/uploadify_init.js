$('input[type=file]').uploadify({
    'uploader'  : '/*{INIT}*/js/uploadify/uploadify.swf',
    'script'    : '/*{HERE}*/',
    'scriptData' : {COOKIE:"/*{COOKIE}*/",HTTP_USER_AGENT:"/*{HTTP_USER_AGENT}*/",'#*#':true},
    'scriptAccess': 'always',
    'queueID': 'uploadifyQueue',
    'cancelImg' : '/*{INIT}*/js/uploadify/cancel.png',
    'buttonImg' : '/*{INIT}*/js/uploadify/button.png',
    'width': 124,
    'height': 30,
    'rollover': true,
    'buttonText': 'Browse Files',
    'checkScript': '?uploadify_check=true',
    'folder'    : '',
    'wmode'        : 'transparent',
    'displayData': 'speed',
    'removeCompleted' : false,
    'auto': true,
    'multi': true,
    /*'fileExt': '{fileExt}',*/
    /*'sizeLimit': {sizeLimit},*/
    'onComplete'  : function(event, ID, fileObj, response, data) {
        var r = $.parseJSON(response);
        var f = $('#'+$(event.currentTarget).attr('id')+ID);
        if(typeof r._status_[1] != 'undefined')
        {
            if(r._status_[1] == true)
            {
                f.attr('class','uploadifyQueueItem completed');
                if(typeof $.fn.dataTable != 'undefined' && typeof r._content_ != 'undefined' && $('#'+$(event.currentTarget).attr('id')+'table').length > 0)
                {
                    var dt = $('#'+$(event.currentTarget).attr('id')+'table').dataTable();
                    var set = false;
                    $.each(dt.fnGetData(),function(k,v){
                        if($.inArray(r._content_[0],v) != -1)
                        {
                            dt.fnUpdate(r._content_,k,0);
                            set = true;
                            return false;
                        }
                    });
                    if(!set)
                    {
                        dt.fnAddData(r._content_);
                    }
                    fervoare & fervoare.xform();
                    $('#'+$(event.currentTarget).attr('id')).uploadifyCancel(ID);
                }
                else
                {
                    $('#'+$(event.currentTarget).attr('id')+ID+' .fileName').html(r._status_[0]);
                }
            }
            else
            {
                f.attr('class','uploadifyQueueItem uploadifyError');
            }
            f.children('.fileName').html(r._status_[0]);
        }
    },
    'onAllComplete':function(event,data){
        fervoare.alert & fervoare.alert('Uploaded '+data.filesUploaded+' file'+(data.filesUploaded!=1?'s':'')+' with '+data.errors+' error'+(data.errors!=1?'s':'')+' at '+Math.round(data.speed*100)/100+'KB/s.');
    }
});    