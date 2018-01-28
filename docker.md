### Install (Ubuntu 16.04)
- Install packages
<pre>
$ sudo apt-get install apt-transport-https ca-certificates curl software-properties-common
</pre>
- Import GPG key
<pre>
$ curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
</pre>
- Add repository
<pre>
$ sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
</pre>
- Install Docker CE (Community Edition)
<pre>
$ sudo apt-get update
$ sudo apt-get install docker-ce
</pre>
- Add configuration to enable docker command without sudo
<pre>
$ sudo groupadd docker
$ sudo usermod -aG docker USERNAME
Once Exit the terminal and open new terminal again.
</pre>
- Check whether install complete successfully
<pre>
docker run --rm hello-world
</pre>
