#!/usr/bin/env ruby

require 'json'

TOKEN = File.read("DIGITAL_OCEAN_TOKEN")

if TOKEN.length < 5
	raise "oops"
end

account_info = JSON.parse(`curl -sSX GET -H "Content-Type: application/json" -H "Authorization: Bearer #{TOKEN}" "https://api.digitalocean.com/v2/account"`)
if account_info["account"]["droplet_limit"] <= 0 then
	raise "oops"
end

new_droplet_info = JSON.parse(`curl -sSX POST -H "Content-Type: application/json" -H "Authorization: Bearer #{TOKEN}" -d '{"name":"guinea","region":"sfo1","size":"512mb","image":"ubuntu-14-04-x64","ssh_keys":["1691002"],"backups":false,"ipv6":false,"user_data":null,"private_networking":null}' "https://api.digitalocean.com/v2/droplets"`)
droplet_id = new_droplet_info["droplet"]["id"]

puts "new droplet created, id: #{droplet_id}"

while true
	droplet_details = JSON.parse(`curl -sSX GET -H "Content-Type: application/json" -H "Authorization: Bearer #{TOKEN}" "https://api.digitalocean.com/v2/droplets/#{droplet_id}"`)
	droplet_status = droplet_details["droplet"]["status"]
	puts droplet_status
	if droplet_status == "active"
		droplet_ip = droplet_details["droplet"]["networks"]["v4"][0]["ip_address"]
		break
	end
end

puts "new droplet ready, ip: #{droplet_ip}"
puts "running ssh"

`ssh -o "StrictHostKeyChecking no" root@#{droplet_ip} "apt-get update; apt-get install --assume-yes git build-essential; git clone https://github.com/zodiac/install.sh.git; cd install.sh; ./make"`