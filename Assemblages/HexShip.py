# Copyright (c) 2011 Eliot Eshelman
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
###############################################################################

import logging
import ogre.renderer.OGRE as ogre

import Application

def create():
    """Create a HexShip entity."""

    ship_id = Application.entity_manager.create_entity()
    logging.debug("HexShip id: " + str(ship_id))

    ogre_entity = Application.ogre_scene_manager.createEntity('Cell', 'HexCell.mesh')
    ogre_node = Application.ogre_root_node.createChildSceneNode(str(ship_id))
    ogre_node.setPosition(ogre.Vector3(0, 0, 0))
    ogre_node.attachObject(ogre_entity)