#!/usr/bin/expect

set timeout 30
spawn ssh root@localhost -p $::env(PORT)
expect "*(yes/no)? " { 
    send "yes\r"
    expect "*?assword" {
        send "sifive\r"
    }
}
expect "# " {
    send "insmod keystone-driver.ko\r"
}
expect "# " {
    send "cd outd\r"
}
expect "# " {
    send "./App.client  Enclave.eapp_riscv eyrie-rt\r"
    expect "main start\r" {} timeout {
        exit 1
    }
    expect "main end\r" {} timeout {
        exit 1
    }
}

expect "# " {
    send "ls -lsa .\r"
}

expect "# " {
    send "./analyzer shared_mem Enclave.nm\r"
    expect "baseaddr: 0x" {} timeout {
        exit 1
    }
}

expect "# " {
    send "poweroff\r"
}

expect eof

exit
