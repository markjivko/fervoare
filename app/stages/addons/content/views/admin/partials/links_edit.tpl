<h1><?php print t(__FILE__,10,'Edit the Link');?></h1>
<?php 
    print form(null,null,array('class'=>'x'));
    print label(t(__FILE__, 11, 'Title'),'title').input(null,'title',$this->v('d','data.title'));
    print label(t(__FILE__, 12, 'Address'),'href').input(null,'href',$this->v('d','data.href'));
    print submit(t(__FILE__, 13, 'Update'));
    print input('hidden','edit-submit',true);
    print form(true);
?>