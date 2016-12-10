# Introduction
A tool to share file via ndn network. It implements a distribute filesystem with high redundancy, which overcomes the disadvantages of bittorrent that old resources are often invailible.

It's based on the model alike a chatroom. Nodes talk in chatroom to exchange control messages.

Chatroom model is implemented based on the NDN project ChronoSync.

It hasn't been test under ndn network with many host.

It now provides a CLI. A FUSE can be implemented later.

# Dependency
- Qt >= 5.7
- ndn-cxx
- boost
- ChronoSync

# Build
You can build it with QtCreator.
