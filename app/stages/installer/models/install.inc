<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko https://markjivko.com
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
/*
 * Install model
 */
class install_m {

    /**
     * Run the installer
     */
    public function update() {
        // Try to connect to the server
        if (false === $instance = mysqli_connect($this->input->post('db_server'), $this->input->post('db_username'), $this->input->post('db_password'))) {
            $this->template->region('error', 'Invalid MySQL credentials.');
            return false;
        }

        // Verify the username
        if ($this->input->post('admin_username') == "") {
            $this->template->region('error', 'Please provide an username for the administrative account.');
            return false;
        }

        // Verify the admin password
        if (strlen($pwd = $this->input->post('admin_password')) < 5) {
            $this->template->region('error', 'Your admin password must be at least 5 characters long.');
            return false;
        }

        // Add the database
        if (false === mysqli_query($instance, sprintf("CREATE DATABASE IF NOT EXISTS %s", $this->input->post('db_data')))) {
            $this->template->region('error', 'Could not create the database.');
            return false;
        }

        // Try to select the database
        if (true !== mysqli_select_db($instance, $this->input->post('db_data', $instance))) {
            $this->template->region('error', 'Could not select the database.');
            return false;
        }

        // Run the install
        if (true === $msg = $this->_install($instance, 'install')) {
            // Hard-code these credentials
            $this->_save(
                $this->input->post('db_server'),
                $this->input->post('db_username'),
                $this->input->post('db_password'),
                $this->input->post('db_data')
            );

            // Create the admin user
            $res = mysqli_query(
                $instance, 
                sprintf(
                    "SELECT * FROM `users` WHERE `username` = '%s'",
                    mysqli_real_escape_string($instance, $this->input->post('admin_username'))
                )
            );

            // Update the admin's password
            if (mysqli_num_rows($res) > 0) {
                mysqli_query(
                    $instance, 
                        sprintf(
                        "UPDATE `users` SET `password` = '%s', `role` = '1' WHERE `username` = '%s'",
                        md5($this->input->post('admin_password')),
                        mysqli_real_escape_string($instance, $this->input->post('admin_username'))
                    )
                );
            }
            // Insert the admin user
            else {
                mysqli_query(
                    $instance, 
                    sprintf(
                        "INSERT INTO `users` SET `password` = '%s', `role` = '1', `username` = '%s'",
                        md5($this->input->post('admin_password')),
                        mysqli_real_escape_string($instance, $this->input->post('admin_username'))
                    )
                );
            }
            return true;
        } else {
            $this->template->region('error', sprintf('The install failed. %s', $msg));
            return false;
        }
    }// end function update()

    /**
     * Hard-code settings
     */
    public function _save($dbServer = null, $dbUser = null, $dbPass = null, $dbName = null) {
        // Load the content of the constants file
        $constants = file_get_contents($f = APP_CONFIG . DS . 'constants' . EXT);

        // Set the definitions
        $this->define = array(
            'DB_SERV' => $dbServer,
            'DB_USER' => $dbUser,
            'DB_PASS' => $dbPass,
            'DB_DATA' => $dbName
        );

        // Perform the replace
        $constants = preg_replace_callback(
            '/define\(\'([^\']*)\',[ ]*\'([^\']*)\'/i',
            array(&$this, '_hardcode_help'),
            $constants
        );

        // Save the file
        $fh = fopen($f, 'w');
        fwrite($fh, $constants);
        fclose($fh);
    }// end function function_name()

    /**
     * Hardcoding replace callback helper
     */
    protected function _hardcode_help($m) {
        if (in_array($m[1], array_keys($this->define))) {
            return "define('{$m[1]}', '{$this->define[$m[1]]}'";
        } else {
            return "define('{$m[1]}', '{$m[2]}'";
        }
    }// end function _hardcode_help()

    /**
     * Run all of the the queries in a .sql script
     * 
     * @example 
     * // Install an SQL file formatted like phpMyAdmin SQL Dump
     * $this->sql->install('backup');
     * // Install a file from another stage
     * $this->sql->install('backup','stageName');
     *
     * @param mysqli $instance
     * @param string $file
     * @param string $stage
     * @return boolean - true on success, false on failure
     */
    protected function _install($instance, $file = null, $stage = null) {
        // The file is mandatory
        if (is_null($file))
            return false;

        // Maybe this needs a lot of time to execute?
        set_time_limit(0);

        // Get the caller's information
        $info = debug_backtrace();
        if (!preg_match('%^.*?\\' . DS . '([^\\' . DS . ']+)\\' . DS . '(controllers|models)\\' . DS . '(.*?)$%', $info[0]['file'], $r)) {
            return 'Please use the original Fervoare CMS installer.';
        }

        // The caller's filename (controller or model)
        $cFile = str_replace(EXT, '', strtolower($r[3]));

        // The caller's stage
        $cStage = $r[1];

        // Set the sql file name
        $file = !is_null($file) ? trim(str_replace('.sql', '', $file)) : $cFile;

        // Set the stage
        $stage = !is_null($stage) ? $stage : $cStage;

        // Set the file path
        $f = preg_replace('%^(.*?)\\' . DS . '([^\\' . DS . ']+)\\' . DS . '(controllers|models)\\' . DS . '(.*?)$%', '$1' . DS . $stage . DS . 'sql' . DS . $file . '.sql', $info[0]['file']);

        // Read the file in lines
        if (file_exists($f)) {
            // Prepare the query string
            $query = '';

            // Run through each line
            foreach (file($f) as $k => $line) {
                // Verify this line is not a comment
                if (!preg_match('/^[ ]*-.*/i', $line)) {
                    $query .= $line;

                    // Was this all?
                    if (substr(rtrim($line), -1) == ';') {
                        // Remove the last character
                        $query = substr($query, 0, strlen($query) - 1);

                        // Run the query
                        if (false === $res = mysqli_query($instance, $query)) {
                            // Something went wrong
                            return sprintf('Verify your syntax: (%s)', $query);
                        }

                        // Clean the string
                        $query = '';
                    }
                }
            }

            // All good :)
            return true;
        } else {
            return 'SQL file ' . $file . ' not found.';
        }
    }// end function install()
}
    
    
/* End Of File <install.inc> */