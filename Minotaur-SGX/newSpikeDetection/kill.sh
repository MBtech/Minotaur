#!/bin/bash
ps aux | grep "./app" | awk '{print $2}' | xargs -I {} kill -9 {}
