<h1>Edit the basic information about your site</h1>
<?php 
    print
    form(null,null,array('class'=>'x')) .
        label('Site Name','site_name').input('text','site_name',$this->v('d','site_name')) .
        label('Motto','motto').input('text','motto',$this->v('d','motto')) .
        label('Twitter account','twitter').input('text','twitter',$this->v('d','twitter')) .
        label('Facebook account','facebook').input('text','facebook',$this->v('d','facebook')) .
        label('Wordpress account','wordpress').input('text','wordpress',$this->v('d','wordpress')) .
        label('Vimeo account','vimeo').input('text','vimeo',$this->v('d','vimeo')) .
        label('Youtube account','youtube').input('text','youtube',$this->v('d','youtube')) .
        label('Tumblr account','tumblr').input('text','tumblr',$this->v('d','tumblr')) .
        label('E-mail account','email').input('text','email',$this->v('d','email')) .
        label('Phone number','phone').input('text','phone',$this->v('d','phone')) .
        submit('Update') .
        form(true);
?>