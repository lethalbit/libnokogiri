Extending libnokogiri
=====================

libnokogiri is designed to be extensible, allowing you to add blocks and options for custom pcapng files, all the while conforming to the pcapng standard.

While admittedly slightly contrived, the process of extending libnokogiri is quite easy by design. There are two things you can add; New blocks, which is described in the :doc:`Adding Blocks </extending/adding_blocks>` section; and new options, which is described in the :doc:`Adding Options </extending/adding_options>` section.

This allows one to leverage the pcapng format in a standards complaint way for other packet based data storage, negating the need for one to develop a custom file format.

.. toctree::
    :hidden:
    :glob:

    *
