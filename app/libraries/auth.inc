<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
class auth_l {
    /**
     * SQL Library
     * 
     * @var sql_cl
     */
    public $sql = null;
    
    /**
     * Session Library
     * 
     * @var session_cl
     */
    public $session = null;
    
    /*
     * User information
     * 
     * @var array|boolean
     */
    public $user = false;

    /*
     * Available roles; the 'admin' key is mandatory
     * @var array
     */
    public $roles = array('admin' => 1, 'user' => 2, 'guest' => 3);

    /*
     * Active time interval; in seconds
     * 2 minutes
     */
    public $active_time = 120;

    /**
     * Constructor
     */
    public function __construct() {
        // Load the session
        $this->session = octoms('session');
        if (!$this->session->is_started()) {
            $this->session->start();
        }

        // Load the sql engine
        $this->sql = octoms('sql');

        // Always refresh the user information
        $this->refresh();
    }

    /**
     * Set the log out flag
     * 
     * @example 
     * // Mark the user for log out
     * {auth}->flag_log_out();
     * // Unmark the user
     * {auth}->flag_log_out(false);
     * 
     * @param boolean $val - Wether to set the flag on or off
     * @param int $id - User id to mark
     * @return boolean - True on success, False on failure
     */
    public function flag_log_out($val = true, $id = null) {
        try {
            // Get the id
            if (is_null($id)) {
                $id = (isset($this->user['id'])) ? $this->user['id'] : 0;
            }

            // The user is not logged in, but there is a "log_out_flag" set
            $this->sql->query(
                $this->sql->update(
                    'users',
                    array(
                        'log_out_flag' => $val ? 1 : 0
                    ),
                    $this->sql->where(
                        array(
                            'id' => $id
                        )
                    )
                )
            );
        } catch (Exception $exc) {
            octoms('error', OMS_CL)->log($exc->getMessage());
            return false;
        }
        
        return true;
    }

    /**
     * Is the user logged in?
     * 
     * @example 
     * // Verify the user is logged in
     * {auth}->is_logged()?'':'';
     * 
     * @return boolean - True if the user is logged in, False otherwise
     */
    public function is_logged() {
        // Get the user first
        $this->user();

        // Return the result
        return !empty($this->user);
    }

    /**
     * Log In
     * 
     * @example
     * // Log the user in
     * {auth}->log_in('foo','bar'); 
     * 
     * @param string $user
     * @param string $password
     * @return boolean - True on success, False on error
     * @throws Exception
     */
    public function log_in($user, $password) {
        if (false !== ($id = $this->verify($user, $password))) {
            // Remove the flag
            $this->flag_log_out(false, $id);

            // Refresh the user id
            return $this->refresh($id);
        }
        
        // The user id is invalid
        return false;
    }

    /**
     * Log Out
     * 
     * @example 
     * // Log out
     * {auth}->log_out();
     */
    public function log_out() {
        // Reset the session information
        $this->session->reset('user');

        // Clear the local variable
        $this->user = false;
    }

    /**
     * Mark the user as active
     * 
     * @example 
     * // Mark the current user as active
     * {auth}->mark_active();
     * 
     * @param string $user
     * @return boolean - True on success, false on failure
     */
    public function mark_active() {
        // If the user is not logged, stop
        if (!$this->is_logged()) {
            return false;
        }

        // Get the last active information
        $last_active = (isset($this->user['last_active'])) ? intval($this->user['last_active']) : 0;

        // Set the user id or username
        $user = $this->user['id'];

        // Update in order?
        if ((time() - $last_active) >= $this->active_time) {
            try {
                // Execute the query
                $this->sql->query(
                    $this->sql->update(
                        'users',
                        array(
                            'last_active' => time(),
                            'last_active_page' => (lang() . '::' . implode(WS, octoms::$oms_url_segments)),
                            'last_ip' => $_SERVER['REMOTE_ADDR']
                        ),
                        $this->sql->where(
                            array(
                                'id' => $user
                            )
                        )
                    )
                );

                // All went well
                return true;
            } catch (Exception $e) {
                flag($e->getMessage());
                return false;
            }
        } else {
            return true;
        }
    }

    /**
     * Refresh user information
     * 
     * @example 
     * // Refresh the user information
     * {auth}->refresh();
     * // Used to log in; provide the ID to load
     * {auth}->refresh(10);
     * // If the user is already logged (ex. user #1), $id will be ignored
     * {auth}->refresh(10); # will refresh the info for id 1
     * 
     * @param int|null $userId User ID
     * @return boolean - True on success, False on failure
     */
    public function refresh($userId = null) {
        // Get the user already logged in
        if (!is_null($userSession = $this->session->get('user'))) {
            $userId = (int) $userSession['id'];
        } else {
            // No id provided?
            if (is_null($userId)) {
                // Log out
                $this->log_out();

                // Bad id
                return false;
            }
        }

        try {
            // Get user information
            $userQuery = $this->sql->query(
                $this->sql->select(
                    null,
                    'users',
                    $this->sql->where(
                        array(
                            'id' => $userId
                        )
                    )
                )
            );
            
            // Any results?
            if ($userQuery->num_rows() > 0) {
                // Get the first result
                $userRow = $userQuery->first();

                // Never store the password
                unset($userRow['password']);
            
                // Forced log out?
                if (1 === (int) $userRow['log_out_flag']) {
                    // Log out
                    $this->log_out();

                    // All done
                    return false;
                } else {
                    // Save info to session
                    $this->session->set('user', $userRow);

                    // Save it to the local variable
                    $this->user = $userRow;

                    // That was it
                    return true;
                }
            } else {
                // Log out
                $this->log_out();

                // Bad user refresh
                return false;
            }
        } catch (Exception $e) {
            // Save this for debugging purposes
            flag($e->getMessage());
            return false;
        }
    }

    /**
     * Register a new user
     * 
     * @param string $username
     * @param string $password
     * @return boolean - True on success, False on error
     */
    public function register($username = '', $password = '') {
        try {
            // Insert query
            $this->sql->query(
                $this->sql->insert(
                    'users',
                    array(
                        'username' => $username,
                        'password' => md5($password)
                    )
                )
            );

            // All went well?
            return (false !== $this->sql->result());
        } catch (Exception $e) {
            // Flag this in case we need it
            flag($e->getMessag());

            // Bad result
            return false;
        }
    }

    /**
     * User role.
     * This is also important for method permissions set with the 'allow' tag in their comment section.
     * There are 3 groups (in descending order by their priority):<ul>
     * <li>admin - [default] the highest priority, access to all methods</li>
     * <li>user - lesser than the admin</li>
     * <li>guest - lesser than the user</li>
     * </ul>
     * Ex.: setting '[at]allow user' in the comment section of a method makes it available for the 'user' group and the 'admin' group only;
     * 
     * @example 
     * // Get the user role
     * {auth}->role();
     * 
     * @return string|boolean - String, user role or False on error
     */
    public function role() {
        // Return the role
        return isset($this->user['role']) ? intval($this->user['role']) : false;
    }

    /**
     * Get the role name
     * 
     * @example 
     * // Get the user's role name
     * {auth}->role_name();
     * 
     * @return string|boolean - String, role name OR False on failure
     */
    public function role_name() {
        if (false !== $role = $this->role()) {
            return array_search(intval($role), $this->roles);
        }
        return false;
    }

    /**
     * User information
     * 
     * @param boolean $asObject - True if the result should be an Object, false for an Array
     * @return array|null - Array user information or null if the user is not set
     */
    public function user($asObject = false) {
        // Populate the user property
        $this->user = $this->session->get('user');

        // Return the information
        if (!is_null($this->user)) {
            return $asObject ? array_to_object($this->user) : $this->user;
        } else {
            return null;
        }
    }

    /**
     * Verify user login credentials
     * 
     * @example 
     * // Verify user credentials for username 'x'
     * {auth}->verify('x','password');
     * // Verify user credentials for user with id 1
     * {auth}->verify(1,'password',true);
     * 
     * @param string|int $user
     * @param string $password
     * @param boolean $useId - wether $user is an ID or a username
     * @return int|boolean User ID or false on failure
     */
    public function verify($user = false, $password = false, $useId = false) {
        // Prepare the query array
        $select = array(
            $useId ? 'id' : 'username' => $user,
            'password' => md5($password),
        );
        
        // Check the credentials
        $this->sql->query(
            $this->sql->select(
                null,
                'users',
                $this->sql->where(
                    $select
                )
            )
        );
        
        // Return the user id
        if ($this->sql->num_rows() > 0) {
            $user = $this->sql->first();
            return (int) $user['id'];
        }
        
        return false;
    }
}

/* End Of File <auth.inc> */