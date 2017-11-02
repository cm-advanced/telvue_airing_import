INTRODUCTION
------------

  This module allows you to import your TV schedule from the Telvue Master Control system, with up to four channels (currently). It allows you to control how often the import process is run, and whether it is automatic.
  
  This module was written in order to decouple the import process from Feeds, as that was making it difficult to update the Feeds module.

  * To submit bug reports and feature suggestions, or to track changes:
    https://github.com/cm-advanced/telvue_airing_import


INSTALLATION
------------

  Install as usual, see https://www.drupal.org/docs/8/extending-drupal-8/installing-contributed-modules-find-import-enable-configure-drupal-8 for further
information.


USAGE
-----

  * Navigate to: admin/config/telvue-airing-import/feeds
  * Set the feed(s) up as needed, and save the form
  * The module will create separate cron tasks for each feed. If you are using Elysia Cron, you can separately configure the exact time that each feed should run.
  * To manually run the import, navigate to: admin/config/telvue-airing-import/import-all
  * To manually delete old data, navigate to: admin/config/telvue-airing-import/delete-all
