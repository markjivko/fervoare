<h2>Edit site accounts</h2>
<?php print link_btn('Add a new user','add');?>
<br/>
<?php if ($this->v('d','others')): $roles = array(1=>'Admin',2=>'User',3=>'Guest'); ?>
<table class="data">
    <thead>
        <tr>
            <th>*</th>
            <th>Username</th>
            <th>Role</th>
            <th>Page</th>
            <th>I.P.</th>
            <th>Last Active</th>
            <th width=200>Actions</th>
        </tr>
    </thead>
    <tbody>
    <?php foreach($this->v('d','others') AS $user):?>
        <tr>
            <td>
                <span class="online <?php if (!$user['log_out_flag']) print 'on';?>">
                <?php print (!$user['log_out_flag'])?'1.On':'2.Off';?>
                <?php print action_btn(
                    'Log Out',
                    $user['id'],
                    'log_out',
                    array(
                        'class'=>'nouniform',
                        'confirm'=>'Are you sure you want to log out '.$user['username'].'?'
                    )
                );?>
                </span></td>
            <td><b><?php print $user['username'];?></b></td>
            <td><?php print $roles[$user['role']];?></td>
            <td><?php 
            $exp = explode('::', $user['last_active_page']);
            if (isset($exp[1]))
            {
                print a($exp[1],i18n_baseurl($exp[1],$exp[0]),array('target'=>'_blank'));
            }
        ?></td>
            <td><?php print a($user['last_ip'],'http://whatismyipaddress.com/ip/'.$user['last_ip'],array('target'=>'_blank'));?></td>
            <td>
            <?php if($user['last_active']!=0):?>
                <span title="<?php print ago($user['last_active']);?>"><?php print date("D, j M Y, H:i:s",$user['last_active']);?><span>
            <?php else:?>
                        Never active
            <?php endif;?>    
                        </td>
                        <td align=center><?php 
            print link_btn('Edit','edit/' .$user['id']);
            print action_btn('Delete',$user['id'],'delete',array('confirm'=>'Are you sure you want to delete this user?'));
        ?></td>
                        </tr>
    <?php endforeach;?>
        </tbody>
    </table>
    <div class="clear"></div>
<?php else:?>
    No other users registered on this site.
<?php endif;?>