# Custom Network Stack

This project implements a custom network stack for Linux, providing functionalities for managing network interfaces, DNS operations, and socket communications.

## Project Structure

```
custom-network-stack
├── src
│   ├── custom_network_stack.c       # Implementation of the custom network stack
│   ├── custom_network_stack.h       # Header file for the custom network stack
│   └── utils
│       ├── dns_utils.c              # Utility functions for DNS operations
│       ├── dns_utils.h              # Header file for DNS utilities
│       ├── socket_utils.c            # Utility functions for socket operations
│       └── socket_utils.h            # Header file for socket utilities
├── include
│   └── common.h                     # Common definitions and utility functions
├── tests
│   ├── test_dns.c                   # Unit tests for DNS utilities
│   ├── test_socket.c                # Unit tests for socket utilities
│   └── Makefile                     # Makefile for compiling and running tests
├── Makefile                         # Makefile for building the project
└── README.md                        # Project documentation
```

## Setup Instructions

1. **Clone the repository:**
   ```
   git clone <repository-url>
   cd custom-network-stack
   ```

2. **Build the project:**
   ```
   make
   ```

3. **Run tests:**
   ```
   cd tests
   make
   ```

## Usage

- To initialize a network interface, use the `custom_initialize_interface` function from `custom_network_stack.c`.
- To configure DNS settings, use the `custom_configure_dns` function.
- For hostname resolution, utilize the `custom_resolve_hostname` function.
- To start a server, call the `custom_start_server` function with the desired protocol and port.

## Contributing

Contributions are welcome! Please submit a pull request or open an issue for any enhancements or bug fixes.

## License

This project is licensed under the MIT License. See the LICENSE file for details.