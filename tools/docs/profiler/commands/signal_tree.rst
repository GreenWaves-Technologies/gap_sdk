Adding/removing Signals
-----------------------

On the right of the Timeline Window, where the signals are being displayed during the simulation of an application, there’s a tree containing all the signals that have been previously selected through the **Settings→ Gvsoc Settings** command. The signals are hierarchically classified. 

Each node of the tree can be **expanded** till we reach full expansion by **left clicking** with the mouse on the arrows of the tree. When a node is expanded, the corresponded new signals are being added in the timeline view.

Each node can be **compressed** by **left clicking** on the same arrow. Corresponding signals will then be removed from the Timeline. 

If the simulation is ongoing or has been fully executed, the display of the signals in the Timeline Window is automatically in sync with the signals tree view. On the following image, the user chose to display the stall signals of the GAP core Pe3. 

.. image:: /source/tools/docs/profiler/commands/images/image_4.png


**Fig 4: Expanding the Signals Tree to display stall signals of core #3**