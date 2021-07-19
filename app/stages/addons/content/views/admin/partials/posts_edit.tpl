<section icon="content.png" name="<?php print t(__FILE__, 27, 'Content');?>">
    <h1><?php print t(__FILE__, 28, 'Edit the content');?></h1>
    <?php 
        print form(null,null,array('class'=>'x')).
        label(t(__FILE__, 29, 'Title'),'title').input(null,'title',$this->v('d','data.title')).
        label(t(__FILE__, 30, 'Slug'),'slug').a(' ','',array('id'=>'slug_link','target'=>'_blank','style'=>($this->v('d','data.status')==1?'':'display:none'))).tag('span',array('id'=>'slug_before'),DOMAIN_PROTOCOL.DOMAIN.i18n_baseurl()).tag('span',array('id'=>'slug_is_blog'),octoms('settings',OMS_L)->get('seo.blog_slug').'/').input(null,'slug',$this->v('d','data.slug')).'<br/>'.
        label(t(__FILE__, 31, 'Type'),'type').select(array(t(__FILE__, 23, 'Article'),t(__FILE__, 24, 'Blog post')),$this->v('d','data.type'),'type').'<br/>'.
        label(t(__FILE__, 32, 'Status'),'status').select(array(t(__FILE__, 25, 'Draft'),t(__FILE__, 26, 'Published')),$this->v('d','data.status'),'status').
        ( lang()!='en' && null!==$this->v('eq_to')?('<br/>'.label(t(__FILE__, 48, 'Translation for'),'eq_to').select($this->v('eq_to'),$this->v('d','data.eq_to'),'eq_to')):'').
        submit(t(__FILE__, 13, 'Update')).
        textarea($this->v('d','data.content'),'content',array('class'=>'edit')).
        input('hidden','edit-submit',true).
        form(true);
    ?>
</section>
<section icon="media.png" name="<?php print t(__FILE__, 33, 'Media');?>">
    <h1><?php print t(__FILE__, 34, 'Upload content from your computer');?></h1>
    <?php octoms::$oms_objects['template_l']->uploadify('posts');?>
</section>
<section icon="js.png" name="JS">
    <h1><?php print t(__FILE__, 35, 'Post JavaScript');?></h1>
    <?php 
        $server = (array)$this->v('d','data.options.js.server');
        $onpage = $this->v('d','data.options.js.onpage');
        print form($this->v('d','data.id').'/js',null,array('class'=>'x')).
        tag('h2',null,t(__FILE__, 36, 'JavaScripts on the server')).textarea(implode("\n", $server),'server',array('class'=>'code')).
        tag('h2',null,t(__FILE__, 37, 'JavaScripts onpage')).textarea($onpage,'onpage').
        tag('br',null,null,true).
        submit(t(__FILE__, 13, 'Update')).
        input('hidden','edit-submit',true).
        form(true);
    ?> 
</section>
<section icon="css.png" name="CSS">
    <h1><?php print t(__FILE__, 38, 'Post CSS');?></h1>
    <?php     
        $server = (array)$this->v('d','data.options.css.server');
        $onpage = $this->v('d','data.options.css.onpage');
        print form($this->v('d','data.id').'/css',null,array('class'=>'x')).
        tag('h2',null,t(__FILE__, 39, 'CSS scripts on the server')).textarea(implode("\n", $server),'server',array('class'=>'code')).
        tag('h2',null,t(__FILE__, 40, 'CSS scripts onpage')).textarea($onpage,'onpage').
        tag('br',null,null,true).
        submit(t(__FILE__, 13, 'Update')).
        input('hidden','edit-submit',true).
        form(true);
    ?> 
</section>
<section icon="seo.png" name="SEO">
    <h1><?php print t(__FILE__, 41, 'Search Engine Optimization');?></h1>
    <?php     
        $keywords = (array)$this->v('d','data.options.seo.keywords');
        $description = $this->v('d','data.options.seo.description');
        print form($this->v('d','data.id').'/seo',null,array('class'=>'x')).
        tag('h2',null,t(__FILE__, 42, 'Keywords (comma separated)')).textarea(implode(',', $keywords),'keywords').
        tag('h2',null,t(__FILE__, 43, 'Description')).textarea($description,'description').
        tag('br',null,null,true).
        submit(t(__FILE__, 13, 'Update')).
        input('hidden','edit-submit',true).
        form(true);
    ?> 
</section>