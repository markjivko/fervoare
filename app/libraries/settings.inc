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

class settings_l {
    /*
     * Data
     * Store the settings as [{table}][{language}][{key}]=>{data}
     */
    public $data;

    /*
     * The language used to get the data
     */
    public $lang;

    /*
     * The table used to get the settings
     */
    public $table;
    
    /**
     * SQL Core Library
     * 
     * @var sql_cl
     */
    public $sql;

    /**
     * Class constructor
     * 
     * @return void
     */
    public function __construct() {
        // Load the sql engine
        if (is_null($this->sql = octoms('sql', OMS_CL))) {
            throw new Exception('SQL Library not loaded or not found.', 500);
        } else {
            if ($this->sql->query("SHOW TABLES LIKE '%settings'")->num_rows() == 0) {
                throw new Exception("Table 'settings' not created.");
            }
        }

        // Set the language
        $this->lang = lang();

        // Set the table
        $this->table = 'settings';
    }

    /**
     * Set the language to use for the settings
     * 
     * @example 
     * // Set the language to "ro"
     * {settings}->setLang('ro');
     * // Revert to the current language
     * {settings}->setLang();
     * 
     * @param string $lang
     * @return settings_l
     */
    public function setLang($lang = null) {
        // Set the language
        $this->lang = is_null($lang) ? lang() : $lang;

        // All done
        return $this;
    }

    /**
     * Set the table name
     * 
     * @example 
     * // Set the table to "foo"
     * {settings}->setTable('foo');
     * // Revert to the default 'settings' table
     * {settings}->setTable();
     * 
     * @param string $table
     * @return settings_l
     */
    public function setTable($table = null) {
        // Set the table
        $this->table = is_null($table) ? 'settings' : $table;

        // All done
        return $this;
    }

    /**
     * Prepare the settings; load multiple setting keys at once
     * 
     * @example 
     * // Prepare "general" and "post" settings
     * {settings}->prep('general','post');
     * {settings}->prep(array('general','post'));
     * 
     * @param array $keys - or  string [$str1][,$str2][...]
     * @return settings_l
     */
    public function prep($keys = '') {
        // The key is mandatory
        if (empty($keys))
            return $this;

        // Multiple arguments?
        if (count($args = func_get_args()) > 1)
            $keys = $args;

        // Prepare the list
        $this->get($keys);

        // Return the settings object
        return $this;
    }

    /**
     * Get a setting from the database
     * 
     * @example 
     * // Get the "general" key
     * {settings}->get('general');
     * // Get the "general.a" subkey
     * {settings}->get('general.a');
     * 
     * // Get "a" and "b" keys
     * {settings}->get(array('a','b'));
     * // ... or
     * {settings}->get('a','b');
     * 
     * // If the key is not found, null is returned
     * {settings}->get('key');
     * // ... or it is not shown in the results list
     * // returns array('key1'=>{value}) if 'key2' is not found
     * {settings}->get(array('key1','key2')); 
     * 
     * @param string $keys
     * @return mixed - the setting value or null if not found; 
     * an array if multiple keys were requested
     */
    public function get($keys = '') {
        // The key is mandatory
        if (empty($keys))
            return;

        // Multiple arguments?
        if (count($args = func_get_args()) > 1)
            $keys = $args;

        // Multiple keys?
        if (is_array($keys)) {
            $processed = array();
            $notSet = array();
            foreach ($keys as $key) {
                // Get the master key and the address 
                $address = explode('.', $key);
                $key = array_shift($address);

                // Save this work
                $processed[] = array($key, count($address) > 0 ? implode('.', $address) : '__self__');

                // Search for unset keys
                if (!isset($this->data[$this->table][$this->lang][$key]) && !in_array($key, $notSet))
                    $notSet[] = $key;
            }
        } else {
            // Get the master key and the address 
            $address = explode('.', $keys);
            $key = array_shift($address);

            // Search for unset keys
            if (!isset($this->data[$this->table][$this->lang][$key]))
                $notSet = $key;
        }

        // Get the information from the database
        if (isset($notSet) && $this->sql->query(
            $this->sql->select(
                null,
                $this->table,
                $this->sql->where(
                    array(
                        'key' => $notSet,
                        'lang' => $this->lang
                    )
                )
            )
        )->num_rows() > 0) {
            foreach ($this->sql->rows() as $row) {
                $this->data[$this->table][$this->lang][$row['key']] = !is_null($row['value']) ? $this->sql->unserialize($row['value']) : null;
            }
        }

        // Return the requested values
        if (is_array($keys)) {
            // This is the result array
            $r = array();

            // Get the key-subkey pairs calculated earlier
            foreach ($processed as $p) {
                // Only list the result if it was found
                isset($this->data[$this->table][$this->lang][$p[0]][$p[1]]) ? ($r[$p[0] . ($p[1] != '__self__' ? ('.' . $p[1]) : '')] = $this->data[$this->table][$this->lang][$p[0]][$p[1]]) : null;
            }

            // Return the result
            return $r;
        }
        // Return a single value
        else {
            // Get the address
            $address = count($address) > 0 ? implode('.', $address) : '__self__';

            // Return null if not found
            return isset($this->data[$this->table][$this->lang][$key][$address]) ? $this->data[$this->table][$this->lang][$key][$address] : null;
        }
    }

    /**
     * Get defined children
     * 
     * @example 
     * // Get all the children of the "general" key
     * {settings}->getChildren('general');
     * 
     * @param string $key
     * @return mixed|null - value, null if not found
     */
    public function getChildren($key = '') {
        // Define the key and subkeys
        $skey = explode('.', $key);
        $key = array_shift($skey);
        $skey = (count($skey) > 0) ? implode('.', $skey) : null;

        // First, populate the data
        $this->get($key);

        // Return the global key
        if (is_null($skey)) {
            // Return all but the __self__ children
            $r = isset($this->data[$this->table][$this->lang][$key]) ? $this->data[$this->table][$this->lang][$key] : null;
            unset($r['__self__']);
            return count((array) $r) > 0 ? $r : null;
        }
        // Or a subkey
        else {
            // Prepare the response 
            $r = array();

            // Is the key set?
            if (!isset($this->data[$this->table][$this->lang][$key]))
                return null;

            // Find the subkeys
            foreach ($this->data[$this->table][$this->lang][$key] as $k => $v) {
                if (0 === strpos($k, $skey) && strlen($k) > strlen($skey)) {
                    // Get only the child part
                    $r[substr($k, strlen($skey) + 1)] = $v;
                }
            }

            // Return the result
            return count($r) > 0 ? $r : null;
        }
    }

    /**
     * Get siblings
     * 
     * @example 
     * // Get all the siblings of the "general.a" key
     * {settings}->getSiblings('general.a');
     * // Get all the settings, if the key is valid
     * {settings}->getSiblings('general');
     * 
     * @param string $key
     * @return array|null The siblings or null if none found
     * 
     */
    public function getSiblings($key = '') {
        // The key is mandatory
        if (empty($key))
            return null;

        // Get the tree structure
        $tree = explode('.', $key);

        // Top-level
        if (count($tree) == 1) {
            // If this is a valid setting, parse the rest of the table
            if (!is_null($this->get($key))) {
                // Get all the master keys that were not yet set
                if ($this->sql->query(
                    $this->sql->select(
                        null,
                        $this->table,
                        $this->sql->where(
                            array(
                                'key' => array_keys((array) $this->data[$this->table][$this->lang]),
                                sql_cl::NOT
                            ),
                            array(
                                'lang' => $this->lang
                            )
                    )
                    )
                )->num_rows() > 0) {
                    foreach ($this->sql->rows() as $row) {
                        $this->data[$this->table][$this->lang][$row['key']] = !is_null($row['value']) ? $this->sql->unserialize($row['value']) : null;
                    }
                }

                // Return all of the keys but the one set
                $r = $this->data[$this->table][$this->lang];
                unset($r[$key]);
                return $r;
            }
        }
        // Find siblings
        else {
            // Go up one level
            $c = array_pop($tree);

            // And find the children; those are the siblings
            $r = $this->getChildren(implode('.', $tree));
            unset($r[$c]);
            return $r;
        }
    }

    /**
     * Set a key
     * 
     * @example 
     * // Set the "general" key as "x"
     * {settings}->set('general','x');
     * // Set the "general.a" subkey
     * {settings}->set('general.a','x');
     * // Set "general.a" and "general.b"
     * {settings}->set(array('general.a'=>'x','general.b'=>'x'));
     * 
     * @param string|array $key
     * @param mixed $value
     * @return settings_l
     * 
     */
    public function set($key = null, $value = null) {
        // The key is mandatory
        if (empty($key))
            return $this;

        // Recursive call
        if (is_array($key)) {
            // Get the current values
            $this->get(array_keys($key));

            // Insert or update?
            $insert = array();
            $update = array();
            foreach ($key as $k => $v) {
                // Get the master key and the address
                $a = explode('.', $k);
                $k = array_shift($a);

                // Prepare the insert array
                if (!in_array($k, array_keys($this->data[$this->table][$this->lang]))) {
                    // Append the key for insert
                    if (!in_array($k, $insert))
                        $insert[] = $k;

                    // Save the new data
                    $this->data[$this->table][$this->lang][$k] = array(count($a) > 0 ? implode('.', $a) : '__self__' => $v);
                } else {
                    // Append the key for update
                    if (!in_array($k, $update))
                        $update[] = $k;

                    // Correct if invalid location
                    if (!is_array($this->data[$this->table][$this->lang][$k]))
                        $this->data[$this->table][$this->lang][$k] = array('__self__' => $this->data[$this->table][$this->lang][$k]);

                    // Append the data
                    $this->data[$this->table][$this->lang][$k][count($a) > 0 ? implode('.', $a) : '__self__'] = $v;
                }
            }
        } else {
            // Get the current value
            $this->get($key);

            // Insert or update?
            $insert = array();
            $update = array();

            // Get the master key and the address
            $a = explode('.', $key);
            $k = array_shift($a);
            $v = $value;

            // Prepare the insert array
            if (!in_array($k, array_keys((array) $this->data[$this->table][$this->lang]))) {
                // Append the key for insert
                if (!in_array($k, $insert))
                    $insert[] = $k;

                // Save the new data
                $this->data[$this->table][$this->lang][$k] = array(count($a) > 0 ? implode('.', $a) : '__self__' => $v);
            } else {
                // Append the key for update
                if (!in_array($k, $update))
                    $update[] = $k;

                // Correct if invalid location
                if (!is_array($this->data[$this->table][$this->lang][$k]))
                    $this->data[$this->table][$this->lang][$k] = array('__self__' => $this->data[$this->table][$this->lang][$k]);

                // Append the data
                $this->data[$this->table][$this->lang][$k][count($a) > 0 ? implode('.', $a) : '__self__'] = $v;
            }
        }

        // Process inserts
        if (count($insert) > 0) {
            // Prepare the values
            $values = array();
            foreach ($insert as $key) {
                $values[] = array(
                    $key,
                    $this->sql->serialize($this->data[$this->table][$this->lang][$key]),
                    $this->lang
                );
            }

            // Perform the insert
            $this->sql->query(
                $this->sql->minsert(
                    $this->table,
                    array(
                        'key',
                        'value',
                        'lang'
                    ),
                    $values
                )
            );
        }
        // Process updates
        elseif (count($update) > 0) {
            // Update each key at a time
            foreach ($update as $key) {
                $this->sql->query(
                    $this->sql->update(
                        $this->table,
                        array(
                            'value' => $this->sql->serialize($this->data[$this->table][$this->lang][$key])
                        ),
                        $this->sql->where(
                            array(
                                'key' => $key,
                                'lang' => $this->lang
                            )
                        )
                    )
                );
            }
        }

        return $this;
    }

    /**
     * Delete a setting
     * 
     * @example 
     * // Unset the 'general' setting
     * {settings}->del('general');
     * // Remove the general's "a" subsetting
     * {settings}->del('general.a');
     * // Remove multiple keys
     * {settings}->del(array('general','general.a'));
     * // ... or
     * {settings}->del('general','general.a');
     * 
     * @param string $address
     * @return settings_l
     */
    public function del($keys = '') {
        // The key is mandatory
        if (empty($keys)) {
            return $this;
        }

        // Multiple arguments?
        if (count($args = func_get_args()) > 1)
            $keys = $args;

        // First, populate the data
        $this->get($keys);

        // Prepare the delete and update arrays
        $delete = array();
        $update = array();

        // Process the key(s)
        if (is_array($keys)) {
            foreach ($keys as $key) {
                // Get the key and address
                $a = explode('.', $key);
                $k = array_shift($a);
                $a = count($a) > 0 ? implode('.', $a) : '__self__';

                // Update or delete?
                if (isset($this->data[$this->table][$this->lang][$k])) {
                    // Is the key valid?
                    $found = isset($this->data[$this->table][$this->lang][$k][$a]) ? true : false;

                    // Remove it
                    unset($this->data[$this->table][$this->lang][$k][$a]);

                    // Delete key from the database
                    if (count($this->data[$this->table][$this->lang][$k]) == 0 && $found) {
                        !in_array($k, $delete) ? ($delete[] = $k) : null;
                    }
                    // Update the database
                    elseif ($found) {
                        $update[$k] = $this->sql->serialize($this->data[$this->table][$this->lang][$k]);
                    }
                }
            }
        } else {
            // Get the key and address
            $a = explode('.', $keys);
            $k = array_shift($a);
            $a = count($a) > 0 ? implode('.', $a) : '__self__';

            // Update or delete?
            if (isset($this->data[$this->table][$this->lang][$k])) {
                // Is the key valid?
                $found = isset($this->data[$this->table][$this->lang][$k][$a]) ? true : false;

                // Remove it
                unset($this->data[$this->table][$this->lang][$k][$a]);

                // Delete key from the database
                if (count($this->data[$this->table][$this->lang][$k]) == 0 && $found) {
                    !in_array($k, $delete) ? ($delete[] = $k) : null;
                }
                // Update the database
                elseif ($found) {
                    $update[$k] = $this->sql->serialize($this->data[$this->table][$this->lang][$k]);
                }
            }
        }

        // Delete keys from the database
        count($delete) > 0 
            ? $this->sql->query(
                $this->sql->delete(
                    $this->table,
                    $this->sql->where(
                        array(
                            'key' => $delete,
                            'lang' => $this->lang
                        )
                    )
                )
            ) 
            : null;

        // Update?
        if (count($update) > 0) {
            // Update each key at a time
            foreach ($update as $key => $value) {
                $this->sql->query(
                    $this->sql->update(
                        $this->table,
                        array(
                            'value' => $value
                        ),
                        $this->sql->where(
                            array(
                                'key' => $key,
                                'lang' => $this->lang
                            )
                        )
                    )
                );
            }
        }

        // All done
        return $this;
    }

    /**
     * Delete children
     * 
     * @example 
     * // Delete all children of the "general" key
     * {settings}->delChildren("general");
     * 
     * @param string $key
     * @return settings_l
     */
    public function delChildren($key = '') {
        // The key is mandatory
        if (empty($key))
            return $this;

        // First, populate the data
        $this->get($key);

        // Get the children
        if (!is_null($ch = $this->getChildren($key))) {
            // Get the names only
            $ch = array_keys($ch);

            // Use the full path
            $ch = array_map(create_function('$k', 'return "' . $key . '.".$k;'), $ch);

            // Delete them all
            call_user_func_array(array($this, 'del'), $ch);
        }

        // All done
        return $this;
    }
}

/* End Of File <settings.inc> */