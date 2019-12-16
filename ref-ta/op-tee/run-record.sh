#!/bin/bash

socat TCP4-LISTEN:54321,reuseaddr - | tee /tmp/tee.log
