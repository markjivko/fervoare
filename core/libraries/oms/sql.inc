<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/**
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
class sql_cl extends octoms {

    /**
     * Long query step (in seconds)
     * All queries that take longer than this to execute will be logged as errors
     * 
     * @var float
     */
    const LONG_QUERY_STEP = 1;
    
    /**
     * More convenient than constants
     * Used for the where clause helper
     * And & Or
     */
    const AND  = 1;
    const OR   = 2;
    const LIKE = '!like!';
    const NOT  = '!not!';

    /**
     * The MySQL connection instance
     * 
     * @var mysqli
     */
    public static $mysqli;

    /**
     * Query cache for the Run method
     * 
     * @var array
     */
    protected static $_cache;

    /**
     * Query result
     * 
     * @var mysqli_result|boolean
     */
    public $_result;

    /**
     * Result rows
     * 
     * @var array|boolean
     */
    protected $_rows = false;

    /**
     * Class constructor
     * 
     * 
     * @param 
     * @return Sql
     * 
     */
    public function __construct() {
        // Verify mysql is installed
        if (!function_exists('mysqli_query')) {
            throw new Exception('MySQL is not installed', 0);
        }

        // Connect to the database
        $this->connect();
    }

    /**
     * Call handler
     */
    public function __call($methodName, $params) {
        // Throw an exception here
        throw new Exception('SQL method "' . $methodName . '" not found.');
    }

    /**
     * Case helper; create a simple mysql CASE query string
     * 
     * @example 
     * // CASE `id` WHEN 1 THEN 3 WHEN 2 THEN 4 ELSE `job` END
     * $this->sql->_case('job','id',array(1=>3,2=>4));
     * 
     * @param string $set
     * @param string $by
     * @param array $cases
     * @return string - query
     */
    protected function _case($set = '', $by = '', $cases = array()) {
        $when = '';
        
        foreach ($cases as $k => $v) {
            $when .= sprintf(
                ' WHEN \'%s\' THEN \'%s\' ',
                $this->escape($k),
                $this->escape($v)
            );
        }
        
        return sprintf(
            'CASE `%s` %s ELSE `%s` END',
            $this->escape($by),
            $when
        );
    }
    
    /**
     * Escape a string
     * 
     * @param string $string
     * @return string
     */
    public function escape($string) {
        return mysqli_real_escape_string(self::$mysqli, $string);
    }

    /**
     * Where helper; create a mysql WHERE query string
     * 
     * @example 
     * // This function uses the Reverse Polish Notation to turn arguments 
     * // into an equation
     *
     * // Return "WHERE ((`name` = 'John' OR `cnp` IN ('1','2','3')) 
     * // AND (`addr` LIKE '%home%' AND `city` LIKE '%b'))"
     * $this->sql->_where(
     *        array('name'=>'John'),
     *        array('cnp'=>array(1,2,3)),
     *        $this->sql->OR,
     *        array(
     *            'addr'=>'%home%',
     *            'city'=>'%b',
     *            $this->sql->LIKE
     *        ),# If the argument has more than 1 key-value pair, the AND operator is used
     *        $this->sql->AND
     *    );
     * // If there are operators missing, they are replaced with AND
     * // Return "WHERE `name` = 'John' AND `cnp` IN ('1','2','3')"
     * $this->sql->_where(
     *        array('name'=>'John'),
     *        array('cnp'=>array(1,2,3))
     *    );
     *
     * 
     * @param [arrays and sql operators]
     * <ul>
     *     <li>AND operator: $this->sql->AND</li>
     *  <li>OR operator: $this->sql->OR</li>
     *  <li>"`field` = 'value'": array('field'=>'value'[,...])</li>
     *  <li>"`field` IN ('1','2')": array('field'=>array(1,2)[,...])</li>
     *  <li>"`field` LIKE '%a%'": array('field'=>'%a%'[,...],'!like')</li>
     * </ul>
     * 
     * @return string - query
     */
    public function where() {
        // Get the arguments
        $args = func_get_args();

        // Return an empty string if there was no input
        if (count($args) == 0)
            return '';

        // Apply the reverse polish notation (rpn)
        $rpn = array();
        foreach ($args as $arg) {
            // Add an element
            if (is_array($arg)) {
                // Perform regex searches?
                if (in_array(self::LIKE, $arg, true)) {
                    $arg = array_diff($arg, array(self::LIKE));
                    $like = true;
                } else {
                    $like = false;
                }

                // Is this a negative?
                if (in_array(self::NOT, $arg, true)) {
                    $arg = @array_diff($arg, array(self::NOT));
                    $not = true;
                } else {
                    $not = false;
                }

                // Multiple fields argument?
                if (count($arg) > 1) {
                    $rpnp = array();
                    foreach ($arg as $k => $v) {
                        // Sanitize the values
                        $v = is_array($v) 
                            ? array_map(array($this, 'escape'), $v) 
                            : $this->escape($v);

                        // Append to the query
                        $rpnp[] = '`' . $this->escape($k) . '` ' . (
                            is_array($v) 
                                ? (
                                    ($not ? 'NOT ' : '') . 'IN (\'' . implode('\',\'', $v) . '\')'
                                ) 
                                : (
                                    $like 
                                    ? ($not ? 'NOT ' : '') . 'LIKE \'' 
                                    : ($not ? '!' : '') . '= \''
                                ) . $v . '\''
                        );
                    }
                    $rpn[] = '(' . implode(' AND ', $rpnp) . ')';
                } elseif (count($arg) == 1) {
                    // Get the key
                    $k = array_keys($arg);
                    $k = $k[0];

                    // Get the values
                    $v = array_values($arg);
                    $v = $v[0];

                    // Sanitize the values
                    $v = is_array($v) 
                        ? array_map(array($this, 'escape'), $v) 
                        : $this->escape($v);

                    // Append to the query
                    $rpn[] = '`' . $this->escape($k) . '` ' . (
                        is_array($v) 
                            ? (
                                ($not ? 'NOT ' : '') . 'IN (\'' . implode('\',\'', $v) . '\')'
                            ) 
                            : (
                                $like 
                                ? ($not ? 'NOT ' : '') . 'LIKE \'' 
                                : ($not ? '!' : '') . '= \''
                            ) . $v . '\''
                    );
                }
            }
            // Add an operator
            elseif (in_array($arg, array(self::AND, self::OR))) {
                $e1 = array_pop($rpn);
                $e2 = array_pop($rpn);
                if (!is_null($e1) && !is_null($e2)) {
                    $rpn[] = ($arg == self::AND)
                        ? ('(' . $e2 . ' AND ' . $e1 . ')')
                        : ('(' . $e2 . ' OR ' . $e1 . ')');
                } elseif (!is_null($e1)) {
                    $rpn = array($e1);
                }
            }
        }

        // If the user did not provide enough operators, fill in with AND
        if (count($rpn) > 1) {
            foreach ($rpn as $k => $arg) {
                if ($k != 0) {
                    $rpn[0] .= ' AND ' . $arg;
                }
            }
        }

        // Return the result
        return isset($rpn[0]) ? 'WHERE ' . $rpn[0] : '';
    }

    /**
     * Simple update helper;
     * 
     * @example 
     * // UPDATE `table` SET (`a`='b') WHERE `id` = '1'
     * $this->sql->_update('table',array('a'=>'b'),$this->sql->_where('id'=>1));
     * 
     * 
     * @param string $table - table name
     * @param array $set
     * @param string $whereClause - generated with $this->_where()
     * @return string - query
     */
    public function update($table = '', $set = array(), $whereClause = '') {
        // Prepare the SQL set
        $sqlSet = array();
        foreach ($set as $k => $v) {
            $sqlSet[] = sprintf(
                '`%s` = \'%s\'',
                $this->escape($k),
                $this->escape($v)
            );
        }

        // Return the simple update string
        return sprintf(
            'UPDATE `%s` SET %s %s',
            $this->escape($table),
            implode(', ', $sqlSet),
            $whereClause
        );
    }

    /**
     * Multiple updates helper; create a mysql UPDATE string on multiple fields with cases
     * 
     * @example 
     * // UPDATE `menu` SET 
     * // `parent` = CASE `id` WHEN 1 THEN 2 WHEN 3 THEN 4 END, 
     * // `weight` = CASE `id` WHEN 4 THEN 3 WHEN 2 THEN 1 END
     * $this->sql->_mupdate('menu',array('parent','id',array(1=>2,3=>4)),array('weight','id',array(4=>3,2=>1)))
     * 
     * @param string $table
     * @param array $switch ({affected field},{switch field},array({case}=>{value}'))
     * @return string - query
     */
    public function mupdate($table = '', $switch = null) {
        // Get the arguments
        $args = func_get_args();

        // Do nothing?
        if (count($args) == 0) {
            throw new Exception('The _mupdate helper requires a minimum of 2 parameters.', 1);
            return false;
        }

        // Get the string ready
        $result = sprintf(
            'UPDATE `%s` SET ',
            $this->escape($set = array_shift($args))
        );

        // Load the cases
        $cases = array();
        foreach ($args as $a) {
            if (count($a) == 3) {
                $cases[] = sprintf(
                    '`%s` = (%s)',
                    $this->escape($a[0]),
                    $this->_case($a[0], $a[1], $a[2])
                );
            }
        }

        // Return the result
        return $result . implode(', ', $cases);
    }

    /**
     * Select helper; create a mysql SELECT string
     * 
     * @example 
     * // SELECT (*) FROM `table`
     * $this->sql->_select(null,'table');
     * // SELECT `a`,`b` FROM `table`
     * $this->sql->_select(array('a','b'),'table');
     * // SELECT `a`, COUNT(`b`) as `count` FROM `table`
     * $this->sql->_select('`a`, COUNT(`b`) as `count`','table');
     * // SELECT * FROM `table` WHERE `a` = 'b'
     * $this->sql->_select(null,'table',$this->sql->_where(array('a'=>'b')));
     * 
     * @param string $fields
     * <ul>
     *     <li>null - *</li>
     *     <li>array() - list of fields</li>
     *     <li>string - a custom list of fields</li>
     * </ul>
     * @param string $table - table name
     * @param string $whereClause - generated by $this->_where()
     * @return string - query
     */
    public function select($fields = null, $table = '', $whereClause = '') {
        // Which fields to select?
        if (empty($fields)) {
            $fields = '*';
        } elseif (is_array($fields)) {
            if (count($fields) > 0) {
                $fields = '`' . implode('`, `', array_map(array($this, 'escape'), $fields)) . '`';
            } else {
                $fields = '*';
            }
        } elseif (!is_string($fields)) {
            $fields = '`' . $this->escape(strval($fields)) . '`';
        }

        // K.i.s.s.
        return sprintf(
            'SELECT %s FROM `%s` %s',
            $fields,
            $this->escape($table),
            $whereClause
        );
    }

    /**
     * Insert helper
     * 
     * @example 
     * // INSERT INTO `table` SET (`a`='1',`b`='2')
     * $this->sql->_insert('table',array('a'=>1,'b'=>2));
     * 
     * 
     * @param string $table - table name
     * @param array $set
     * @param string $whereClause
     * @return string - query
     */
    public function insert($table = '', $set = array()) {
        $sqlSet = array();
        foreach ($set as $k => $v) {
            $sqlSet[] = sprintf('`%s` = \'%s\'',
                $this->escape($k),
                $this->escape($v)
            );
        }

        // Return the insert query
        return sprintf(
            'INSERT INTO `%s` SET %s',
            $this->escape($table),
            implode(', ', $sqlSet)
        );
    }

    /**
     * Multiple insert helper
     * 
     * @example 
     * // Perform multiple inserts
     * $this->sql->_minsert('tableName',array('field1','field2'),array(array('foo','bar'),array('bar','baz')[,...]));
     * 
     * @param string $table
     * @param array $keys
     * @param array $values
     * @return string - query
     */
    public function minsert($table = '', $keys = array(), $values = array()) {
        // Sanitize the keys
        $keys = array_map(array($this, 'escape'), $keys);
        $keyCount = count($keys);

        // Prepare the values for the insert
        foreach ($values as $k => $v) {
            // This must be an array
            if (!is_array($v))
                $v = (array) $v;

            // The same size as the keys array
            if (count($v) == $keyCount) {
                $values[$k] = sprintf(
                    '\'%s\'',
                    implode("', '", array_map(array($this, 'escape'), $v))
                );
            } else {
                unset($values[$k]);
            }
        }

        // Return the string
        return sprintf(
            'INSERT INTO `%s` (`%s`) VALUES (%s)',
            $this->escape($table),
            implode("`, `", $keys),
            implode("), (", $values)
        );
    }

    /**
     * Delete helper
     * 
     * @example 
     * // DELETE * FROM `table`
     * $this->sql->_delete('table');
     * 
     * 
     * @param string $table - table name
     * @param string $whereClause
     * @return string - query
     */
    public function delete($table = '', $whereClause = '') {
        // Return the delete query
        return sprintf(
            'DELETE FROM `%s` %s',
            $this->escape($table),
            $whereClause
        );
    }

    /**
     * Connector
     * 
     * @example $this->sql->connect('localhost','root','');
     * 
     * @param string $server
     * @param string $username
     * @param string $password
     * @return resource|boolean
     */
    public function connect($server = DB_SERV, $username = DB_USER, $password = DB_PASS) {
        if (!isset(self::$mysqli)) {
            if ((self::$mysqli = mysqli_connect($server, $username, $password, false)) === false) {
                throw new Exception('Could not connect to the database.', 500);
                return false;
            } else {
                // Set encoding to UTF8
                mysqli_set_charset(self::$mysqli, 'utf8');

                // Select the database
                $this->db();
            }
        }
        
        return $this;
    }

    /**
     * Select the database
     * 
     * @param string $name
     * @return Sql|boolean
     */
    public function db($name = DB_DATA) {
        // Select the database
        if (!empty($name)) {
            if (mysqli_select_db(self::$mysqli, $name) === true) {
                return $this;
            } else {
                throw new Exception(sprintf('Could not select database "%s".', $name), 500);
            }
        } else {
            throw new Exception('You must specify a database to select.', 500);
        }

        return false;
    }

    /**
     * Disconnector
     * 
     * @example $this->sql->disconnect();
     * 
     * @return boolean
     * 
     */
    public function disconnect() {
        if (isset(self::$mysqli) && self::$mysqli !== false) {
            return mysqli_close(self::$mysqli);
        } else {
            return false;
        }
    }

    /**
     * Serialize
     * 
     * @example 
     * # $var = array('a','b','c'=>array('d','e'));
     * # $str = "eF5LtDK2qs60MrAutjK0UkpUss60MoSwk5QgdLKSdaKVEZKiFCRFqUrWtbUAbPsSHA=="
     * $ser = $this->sql->serialize($var);
     * 
     * @param mixed $var 
     * @return string - serialized and compressed 
     * 
     */
    public function serialize($var = null) {
        return @base64_encode($this->compress(serialize($var)));
    }

    /**
     * Unserialize
     * 
     * @example 
     * # $str = "eF5LtDK2qs60MrAutjK0UkpUss60MoSwk5QgdLKSdaKVEZKiFCRFqUrWtbUAbPsSHA=="
     * # $var = array('a','b','c'=>array('d','e'));
     * $var = $this->sql->unserialize($str);
     * 
     * @param string $str
     * @return mixed - unserialized value or false on failure
     */
    public function unserialize($str = '') {
        // Get the unserialized data
        $data = @unserialize($str = $this->decompress(base64_decode($str)));

        // Check if it is real
        if ($str === 'b:0;' || $data !== false) {
            return $data;
        } else {
            return null;
        }
    }

    /**
     * Compress
     * 
     * @example 
     * # $str = "Test string. test string. test string.";
     * # $res = "eF4LSS0uUSguKcrMS9dTKMHFAQAThg4Q";
     * $res = $this->sql->compress($str);
     * 
     * @param string $var 
     * @return string - base 64 compressed string
     * 
     */
    public function compress($str = '') {
        if (false === ($str = @gzcompress($str, 4))) {
            return false;
        } else {
            return @base64_encode($str);
        }
    }

    /**
     * Decompress
     * 
     * @example 
     * # $str = "eF4LSS0uUSguKcrMS9dTKMHFAQAThg4Q";
     * # $res = "Test string. test string. test string.";
     * $res = $this->sql->decompress($str);
     * 
     * @param string $str - base 64
     * @return string
     * 
     */
    public function decompress($str = '') {
        if (false === ($str = @base64_decode($str))) {
            return false;
        } 
        
        return strlen($str) > 0 ? @gzuncompress($str) : false;
    }

    /**
     * Runs an SQL statement from an .sql file
     * 
     * @example $this->sql->run('stage:file:query name'); # run the query "query name" from stage/sql/file.sql
     *     $this->sql->run('file:query name'); # run the query "query name" from {current stage}/sql/file.sql
     *     $this->sql->run('query name',$param1,$param2...); # run the query "query name" from {current stage}/sql/{current model or controller}.sql with given parameters
     * 
     * @param string $sqlLocation
     * @param string|int undefined number of arguments
     * @return sql_cl
     */
    public function run() {
        // Get all the function arguments
        $p = func_get_args();

        // Determine the sql credentials
        if (empty($p)) {
            throw new Exception('Please specify the name of the SQL query to run.');
            return false;
        }

        // Have we cached the query?
        if (!isset(self::$_cache[$p[0]])) {
            // Transform the sql {stage name}:{sql file}:{sql query} to an array
            $sqlArr = array_reverse(explode(':', $p[0]));

            // Get the caller's information
            $info = debug_backtrace();
            if (!preg_match('%^.*?\\' . DS . '([^\\' . DS . ']+)\\' . DS . '(controllers|models)\\' . DS . '(.*?)$%', $info[0]['file'], $r)) {
                return false;
            }

            // The caller's filename (controller or model)
            $cFile = str_replace(EXT, '', strtolower($r[3]));

            // The caller's stage
            $cStage = $r[1];

            // Filter the query name
            $query = preg_replace('/[^0-9a-zA-Z\s\-\_]/i', '', trim($sqlArr[0]));

            // Set the sql file name
            $file = isset($sqlArr[1]) ? trim(str_replace(EXT, '', strtolower($sqlArr[1]))) : $cFile;

            // Set the stage name
            $stage = isset($sqlArr[2]) ? trim(strtolower($sqlArr[2])) : $cStage;

            // Have we cached the sql file?
            if (!isset(self::$_cache['__files__'][$stage . DS . $file])) {
                // Set the sql file location
                $f = preg_replace('%^(.*?)\\' . DS . '([^\\' . DS . ']+)\\' . DS . '(controllers|models)\\' . DS . '(.*?)$%', '$1' . DS . $stage . DS . 'sql' . DS . $file . '.sql', $info[0]['file']);

                // Finally, store this file to the cache
                if (file_exists($f)) {
                    // Read the contents
                    self::$_cache['__files__'][$stage . DS . $file] = file_get_contents($f);
                } else {
                    throw new Exception(sprintf('Sql file (%s) not found.', $f));
                    return false;
                }
            }

            // Get the SQL file contents
            $sql = self::$_cache['__files__'][$stage . DS . $file];

            // Extract the SQL statement we are looking for
            if (preg_match('%(--\s*<(q|query){1}\s*["\']{1}' . $query . '["\']{1}\s*>)([^\e]*?)(--\s*</(q|query){1})%i', $sql, $reg)) {
                // Remove all comments
                self::$_cache[$p[0]] = preg_replace('%\s*--.*|\s*/\*([^\e]*)\*/%', '', $reg[3]);

                // Clear the memory
                unset($sql);
            } else {
                throw new Exception(sprintf('Query (%s) not found in {stages}%s.', $query, DS . $stage . DS . 'sql' . DS . $file . '.sql'));
                return false;
            }
        }

        // Prepare the argument list for the query
        $p[0] = self::$_cache[$p[0]];

        // Execute the query
        return call_user_func_array(array($this, 'query'), $p);
    }

    /**
     * Run all of the the queries in a .sql script
     * 
     * @example 
     * // Install an SQL file formatted like phpMyAdmin SQL Dump
     * $this->sql->install('backup');
     * // Install a file from another stage
     * $this->sql->install('backup','stageName');
     *
     * @param string $file
     * @param string $stage
     * @return boolean - true on success, false on failure
     */
    public function install($file = null, $stage = null) {
        // The file is mandatory
        if (is_null($file))
            return false;

        // Maybe this needs a lot of time to execute?
        set_time_limit(0);

        // Get the caller's information
        $info = debug_backtrace();
        if (!preg_match('%^.*?\\' . DS . '([^\\' . DS . ']+)\\' . DS . '(controllers|models)\\' . DS . '(.*?)$%', $info[0]['file'], $r)) {
            return false;
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
                        $this->query($query);

                        // Clean the string
                        $query = '';
                    }
                }
            }

            return true;
        }
        
        return false;
    }

    /**
     * Query the database
     * 
     * @example $this->sql->query("SELECT * FROM `%s`",$tableName); # similar to sprintf()
     * @return sql_cl
     */
    public function query() {
        // Starting from scratch
        $this->_result = false;
        $this->_rows = false;

        // Get the function arguments
        $args = func_get_args();

        // Get the query string
        $querySting = array_shift($args);

        // Check for extra parameters
        if (!empty($args)) {
            // Sanitize input
            foreach ($args as $k => $v) {
                if (is_array($v) || is_object($v)) {
                    $args[$k] = $this->serialize($v);
                } else {
                    $args[$k] = $this->escape($v);
                }
            }

            // Verify the semantics
            if (substr_count($querySting, '%s') > count($args)) {
                throw new Exception('Query arguments count and function parameters count do not match.');
                return false;
            }

            // Sanitize the input
            array_walk($args, array($this, 'escape'));

            // Use the custom table prefix; add the query to the args list
            array_unshift($args, str_replace('{pref}', DB_PREF, $querySting));

            // Use arguments
            $querySting = call_user_func_array('sprintf', $args);
        }

        // Debugging?
        if (debugging() && !ENVIRONMENT) {
            // Initialize the SQL info array
            if (!isset(octoms::$oms_info['sql'])) {
                octoms::$oms_info['sql'] = array(
                    'n' => 0,
                    't' => 0,
                    'q' => array()
                );
            }

            // Get the start time
            $sqlTime = microtime(true);

            // Execute the query
            $this->_result = mysqli_query(self::$mysqli, $querySting);

            // Total execution time
            $sqlTime = number_format(microtime(true) - $sqlTime, 6);

            // Save the data
            octoms::$oms_info['sql']['n']++;
            octoms::$oms_info['sql']['t'] += $sqlTime;
            $data = array(
                'query' => $querySting,
                'execution' => $sqlTime . ' seconds',
            );
            foreach (debug_backtrace() as $i => $d) {
                if (isset($d['file']) && isset($d['line']) && strpos($d['file'], 'octoms' . EXT) === false && strpos($d['file'], 'index.php') === false) {
                    $data['trace:' . $i] = str_replace(DS, '/', $d['file']) . ', line ' . $d['line'];
                }
            }
            octoms::$oms_info['sql']['q'][] = $data;
        } else {
            // Get the start time
            $sqlTime = microtime(true);

            // Execute the query
            $this->_result = mysqli_query(self::$mysqli, $querySting);

            // Total execution time
            $sqlTime = microtime(true) - $sqlTime;

            // Long query?
            if ($sqlTime >= sql_cl::LONG_QUERY_STEP) {
                // Get the caller information
                $info = debug_backtrace();

                // Get the file and line
                if (isset($info[0]['file'])) {
                    $file = $info[0]['file'];
                    $line = $info[0]['line'];
                } else {
                    $file = __FILE__;
                    $line = __LINE__;
                }

                // Log this as an error
                octoms('error', OMS_CL)->log(
                    sprintf(
                        'Long query (%ss): "%s"',
                        number_format($sqlTime, 6),
                        $querySting
                    ),
                    $file,
                    $line
                );
            }
        }

        // Something went wrong
        if ($this->_result === false) {
            throw new Exception(sprintf('Verify your syntax: %s', $querySting) . ', ' . self::$mysqli->error);
        }

        // All good
        return $this;
    }

    /**
     * Count the number of rows a SQL result has
     * 
     * @example $result->num_rows();
     * @return int|boolean Number of rows or false on error
     */
    public function num_rows() {
        return $this->_result instanceof mysqli_result ? mysqli_num_rows($this->_result) : false;
    }

    /**
     * Last insert id
     * 
     * @example $result->insert_id();
     * @return int|boolean
     */
    public function insert_id() {
        return ($this->_result !== false) ? mysqli_insert_id(self::$mysqli) : false;
    }

    /**
     * Parse the return array
     * 
     * @example foreach($result->rows() as $key=>value)... 
     * @return array|boolean Rows on success, false on failure
     */
    public function rows() {
        // Initialize the rows
        if ($this->_result instanceof mysqli_result) {
            if (false === $this->_rows) {
                $this->_rows = array();
                while ($row = mysqli_fetch_assoc($this->_result)) {
                    $this->_rows[] = $row;
                }
            }
        }
        return $this->_rows;
    }

    /**
     * Return the result resource
     * 
     * @example $this->sql->result();
     * $this->sql->query("...")->result();
     * 
     * @return mysqli_result|boolean <br/>
     * Returns the resource or false if the query failed.
     */
    public function result() {
        return $this->_result;
    }

    /**
     * Return the first element of the result array
     * 
     * @example $result->first();
     * @return array|boolean
     */
    public function first() {
        return is_array($this->rows()) && count($this->rows())
            ? reset($this->_rows)
            : false;
    }

    /**
     * Return the last element of the result array
     * 
     * @example $result->last();
     * @return array|boolean
     */
    public function last() {
        return is_array($this->rows()) && count($this->rows())
            ? end($this->_rows)
            : false;
    }

    /**
     * Return the next element of the result array
     * 
     * @example $result->prev();
     * @return array|boolean
     */
    public function next() {
        return is_array($this->rows()) && count($this->rows())
            ? next($this->_rows)
            : false;
    }

    /**
     * Return the previous element of the result array
     * 
     * @example $result->next();
     * @return array|boolean false
     */
    public function prev() {
        return is_array($this->rows()) && count($this->rows())
            ? prev($this->_rows)
            : false;
    }
}
    
/* End Of File <sql.inc> */