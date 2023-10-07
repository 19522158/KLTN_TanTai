DATE=`date '+%Y-%m-%d %H:%M:%S'`
echo "Example service started at ${DATE}" | systemd-cat -p info
while true
do
	/var/www/html/live/kltn1
	sleep 1
done
