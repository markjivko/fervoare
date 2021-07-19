<section name="General" icon="general.png">
    <h2>General site settings</h2>
    <?php 
        print form(null,null,array('class'=>'x')).
        label('Page extension default','general_extension').input('text','general_extension',$this->v('d','general_extension')).
        submit('Update').
        form(true);
    ?>
</section>
<section name="Site front" icon="site-front.png">
    <h2>Meta information on the front page of your site</h2>
    <?php print
        form(null,null,array('class'=>'x')).
        label('Title','site_title').input('text','site_title',$this->v('d','site_title')).
        label('Keywords','site_keywords').input('text','site_keywords',implode(',',(array)$this->v('d','site_keywords'))).
        label('Description','site_description').textarea($this->v('d','site_description'),array('site_description')).
        submit('Update').
        form(true);
    ?>
</section>
<section name="Blog front" icon="blog-front.png">
    <h2>Meta information on the front page of your site's blog</h2>
    <?php print
        form(null,null,array('class'=>'x')).
        label('Title','blog_title').input('text','blog_title',$this->v('d','blog_title')).
        label('Slug','blog_slug').input('text','blog_slug',$this->v('d','blog_slug')).
        label('Keywords','blog_keywords').input('text','blog_keywords',implode(',', (array)$this->v('d','blog_keywords'))).
        label('Description','blog_description').textarea($this->v('d','blog_description'),array('blog_description')).
        submit('Update').
        form(true);
    ?>
</section>
<section name="Google Analytics" icon="ga.png">
    <h2>Set the Google Analytics tracking id</h2>
    <?php print
        form(null,null,array('class'=>'x')).
        label('Google Analytics ID','ga_id').input('text','ga_id',$this->v('d','ga_id')).
        submit('Update').
        form(true);
    ?>
</section>