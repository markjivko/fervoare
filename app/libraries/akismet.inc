<?php if (! defined ( 'OCTOMS' )){header ( 'HTTP/1.1 403' );die ( '{"error":"forbidden"}' );}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

class akismet_l {
    public $errors = array();
    public $akismetVersion = '1.1';
    public $con;
    public $host;
    public $port;
    public $apiPort = 80;
    public $akismetServer = 'rest.akismet.com';
    public $http;
    public $ignore = array(
        'HTTP_COOKIE', 'HTTP_X_FORWARDED_FOR', 'HTTP_X_FORWARDED_HOST', 
        'HTTP_MAX_FORWARDS', 'HTTP_X_FORWARDED_SERVER', 'REDIRECT_STATUS', 
        'SERVER_PORT', 'PATH', 'DOCUMENT_ROOT', 'SERVER_ADMIN', 'QUERY_STRING', 
        'PHP_SELF', 'argv'
    );
    public $blogUrl = "";
    public $apiKey = "";
    public $comment = array();

    const AKISMET_SERVER_NOT_FOUND = 0;
    const AKISMET_RESPONSE_FAILED = 1;
    const AKISMET_INVALID_KEY = 2;

    /**
     * Initialize the Akismet; Set instance variables, connect to Akismet, and check API key
     * 
     * @example
     * // The comment must look like this
     * $comment = array(
     * 'author'    => 'foo',
     * 'email'     => 'bar[at]example.com',
     * 'website'   => 'http://www.example.com/',
     * 'body'      => 'Baz',
     * 'permalink' => 'http://yourdomain.com/yourblogpost.url',
     * );
     * // Initialize the library 
     * $this->akismet->init(
     * 'http://www.yourdomain.com/', 
     * 'YOUR_WORDPRESS_API_KEY', 
     * $comment
     * );
     * // Check for errors
     * if($this->akismet->errorsExist()) 
     * {
     * // Could not connect to the server
     * } 
     * else
     * {
     * // Check if the comment is spam
     * if($this->akismet->isSpam())
     * {
     * // Spam detected
     * } 
     * else
     * {
     * // No spam
     * }
     * }
     * 
     * @param    String    $blogUrl    The URL to your own blog
     * @param     String    $apiKey        Your wordpress API key
     * @param     String[]    $comment    A formatted comment array to be examined by the Akismet service
     * @return    Akismet
     */
    public function init($blogUrl = null, $apiKey = null, $comment = array()) {
        $this->blogUrl = $blogUrl;
        $this->apiKey = $apiKey;
        $this->setComment($comment);

        // Connect to the Akismet server and populate errors if they exist
        $this->http = $this->AkismetHttpClient($this->akismetServer, $blogUrl, $apiKey);
        if ($this->http->errorsExist()) {
            $this->errors = array_merge($this->errors, $this->http->getErrors());
        }

        // Check if the API key is valid
        if (!$this->_isValidApiKey($apiKey)) {
            $this->setError(self::AKISMET_INVALID_KEY, "Your Akismet API key is not valid.");
        }
    }

    /**
     * Query the Akismet and determine if the comment is spam or not
     * 
     * @return    boolean
     */
    public function isSpam() {
        $response = $this->http->getResponse($this->_getQueryString(), 'comment-check');

        return ($response == "true");
    }

    /**
     * Submit this comment as an unchecked spam to the Akismet server
     * 
     * @return    void
     */
    public function submitSpam() {
        $this->http->getResponse($this->_getQueryString(), 'submit-spam');
    }

    /**
     * Submit a false-positive comment as "ham" to the Akismet server
     *
     * @return    void
     */
    public function submitHam() {
        $this->http->getResponse($this->_getQueryString(), 'submit-ham');
    }

    /**
     * Manually set the comment value of the instantiated object.
     *
     * @param    Array    $comment
     * @return    void
     */
    public function setComment($comment) {
        $this->comment = $comment;
        if (!empty($comment)) {
            $this->_formatCommentArray();
            $this->_fillCommentValues();
        }
    }

    /**
     * Returns the current value of the object's comment array.
     *
     * @return    Array
     */
    public function getComment() {
        return $this->comment;
    }

    /**
     * Add an error to the errors array in the object
     *
     * @param    String    $name    A name (array key) for the error
     * @param    String    $string    The error message
     * @return void
     */
    // Set an error in the object
    public function setError($name, $message) {
        $this->errors [$name] = $message;
    }

    /**
     * Return a specific error message from the errors array
     *
     * @param    String    $name    The name of the error you want
     * @return mixed    Returns a String if the error exists, a false boolean if it does not exist
     */
    public function getError($name) {
        if ($this->isError($name)) {
            return $this->errors [$name];
        } else {
            return false;
        }
    }

    /**
     * Return all errors in the object
     *
     * @return String[]
     */
    public function getErrors() {
        return (array) $this->errors;
    }

    /**
     * Check if a certain error exists
     *
     * @param    String    $name    The name of the error you want
     * @return boolean
     */
    public function isError($name) {
        return isset($this->errors [$name]);
    }

    /**
     * Check if any errors exist
     *
     * @return boolean
     */
    public function errorsExist() {
        return (count($this->errors) > 0);
    }

    /**
     * The Akismet HTTP Client
     */
    private function AkismetHttpClient($host, $blogUrl, $apiKey, $port = 80) {
        $this->host = $host;
        $this->port = $port;
        $this->blogUrl = $blogUrl;
        $this->apiKey = $apiKey;
    }

    /**
     * Use the connection active in $con to get a response from the server and return that response
     */
    private function getResponse($request, $path, $type = "post", $responseLength = 1160) {
        $this->_connect();

        if ($this->con && !$this->isError(self::AKISMET_SERVER_NOT_FOUND)) {
            $request = strToUpper($type) . " /{$this->akismetVersion}/$path HTTP/1.0\r\n" . "Host: " . ((!empty($this->apiKey)) ? $this->apiKey . "." : null) . "{$this->host}\r\n" . "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n" . "Content-Length: " . strlen($request) . "\r\n" . "User-Agent: Akismet PHP4 Class\r\n" . "\r\n" . $request;
            $response = "";

            @fwrite($this->con, $request);

            while (!feof($this->con)) {
                $response .= @fgets($this->con, $responseLength);
            }

            $response = explode("\r\n\r\n", $response, 2);
            return $response [1];
        } else {
            $this->setError(self::AKISMET_RESPONSE_FAILED, "The response could not be retrieved.");
        }

        $this->_disconnect();
    }

    /**
     * Connect to the Akismet server and store that connection in the instance variable $con
     */
    private function _connect() {
        if (!($this->con = @fsockopen($this->host, $this->port))) {
            $this->setError(self::AKISMET_SERVER_NOT_FOUND, "Could not connect to akismet server.");
        }
    }

    /**
     * Close the connection to the Akismet server
     */
    private function _disconnect() {
        @fclose($this->con);
    }

    /**
     * Check with the Akismet server to determine if the API key is valid
     *
     * @access    Protected
     * @param    String    $key    The Wordpress API key passed from the constructor argument
     * @return    boolean
     */
    private function _isValidApiKey($key) {
        $keyCheck = $this->http->getResponse("key=" . $this->apiKey . "&blog=" . $this->blogUrl, 'verify-key');

        return ($keyCheck == "valid");
    }

    /**
     * Format the comment array in accordance to the Akismet API
     *
     * @access    Protected
     * @return    void
     */
    private function _formatCommentArray() {
        $format = array('type' => 'comment_type', 'author' => 'comment_author', 'email' => 'comment_author_email', 'website' => 'comment_author_url', 'body' => 'comment_content');

        foreach ($format as $short => $long) {
            if (isset($this->comment [$short])) {
                $this->comment [$long] = $this->comment [$short];
                unset($this->comment [$short]);
            }
        }
    }

    /**
     * Fill any values not provided by the developer with available values.
     *
     * @return    void
     */
    private function _fillCommentValues() {
        if (!isset($this->comment ['user_ip'])) {
            $this->comment ['user_ip'] = ($_SERVER ['REMOTE_ADDR'] != getenv('SERVER_ADDR')) ? $_SERVER ['REMOTE_ADDR'] : getenv('HTTP_X_FORWARDED_FOR');
        }
        if (!isset($this->comment ['user_agent'])) {
            $this->comment ['user_agent'] = $_SERVER ['HTTP_USER_AGENT'];
        }
        if (!isset($this->comment ['referrer'])) {
            $this->comment ['referrer'] = $_SERVER ['HTTP_REFERER'];
        }
        if (!isset($this->comment ['blog'])) {
            $this->comment ['blog'] = $this->blogUrl;
        }
    }

    /**
     * Build a query string for use with HTTP requests
     *
     * @access    Protected
     * @return    String
     */
    private function _getQueryString() {
        foreach ($_SERVER as $key => $value) {
            if (!in_array($key, $this->ignore)) {
                if ($key == 'REMOTE_ADDR') {
                    $this->comment [$key] = $this->comment ['user_ip'];
                } else {
                    $this->comment [$key] = $value;
                }
            }
        }

        $query_string = '';

        foreach ($this->comment as $key => $data) {
            $query_string .= $key . '=' . urlencode(stripslashes($data)) . '&';
        }

        return $query_string;
    }
}

/* End Of File <akismet.inc> */