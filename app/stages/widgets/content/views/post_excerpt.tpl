<?php foreach ($this->v('d') AS $post):?>
    <h2><?php print a($post['title'],i18n_baseurl($this->v('blog').'/'.$post['slug']));?></h2>
    <p><?php print $post['content'];?></p>
    <hr/>
<?php endforeach;?>