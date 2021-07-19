--<q "eq_to_free">
SELECT `p1`.* 
FROM `posts` AS `p1`
WHERE    `p1`.`lang` = 'en'
    AND 
    (
        `p1`.`id` NOT IN 
        (
            SELECT `eq_to`
            FROM `posts` 
            WHERE `eq_to` IS NOT NULL
            OR `eq_to` != '0'
        )
        OR `p1`.`id` = '%s'
    )
--</q>