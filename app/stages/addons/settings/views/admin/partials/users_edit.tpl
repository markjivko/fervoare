<h2>Edit account information for user #<?php print $this->v('data','id');?></h2>
<?php
    // Get the authentification library
    $auth = octoms('auth',OMS_L);
    
    // Print the form
    print form(null,null,array('class'=>'x')).
    label('Username','username').input('text','username',$this->v('data','username')).
    label('Name','name').input('text','name',$this->v('data','name')).
    label('E-mail','email').input('text','email',$this->v('data','email')).
    // Set the password
    ($auth->user['role'] != 1 ?
        label('Old Password','old_password').input('password','old_password','1234')
        :
        null
    ).
    
    label('New Password','new_password').input('password','new_password','',array('autocomplete'=>'off')).
    // Only admins are allowed to edit the roles
    ($auth->user['role'] == 1 ?
        label('Role','role').select(array('1'=>'Admin','2'=>'User','3'=>'Guest'),$this->v('data','role'),array('name'=>'role'))
        :
        null
    ).
    submit('Update').
    form(true);
?>