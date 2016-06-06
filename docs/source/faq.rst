
.. _faq:

FAQ
===

Here are some questions and answers I came up with, which potentially answer some of the ones that
will eventually come up.


Q: WHY?!
^^^^^^^^

**A**: The motivation for this project was mainly experimentation. If you look at today's JavaScript runtimes they
seem to have a similar model: on one hand we have browsers and on the server side we have Node, but both follow
a similar event-driven model. Skookum JS follows the more "traditional" model of providing all the low level
primitives required to build different kinds of abstractions, an event-driven model being just one of them.


Q: Does it work on <insert your OS of choice>?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**A**: At the moment sjs works only on GNU/Linux and OSX. Adding support for other Unix platforms should be
easy enough and contributions are more than welcome. Windows support is not currently planned in the short term.


Q: Do Nodejs modules work with Skookum JS?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**A**: Most likely not. Some of them might, but if a module uses any Node or browser specifics (such as ``console``
or ``process``) it won't. In addition, sjs does not read ``package.json`` files nor search for modules in
``node_mules`` directories. This is by design.


Q: How can I install modules?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**A**: At the moment there is no packaging system, so putting them in a directory in the sjs search path is the only
option. Let's say ``cp`` is our current package manager :-)

