<?php if(!defined('OCTOMS')){header('HTTP/1.1 403'); die('{"error":"forbidden"}');}?>
Error #<?php echo $info->no;?> occured (<?php echo $info->errors[$info->no];?>).
<?php echo $info->message;?>