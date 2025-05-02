function download(){
    console.log('Download button clicked!');
document.getElementById('downloadBtn').addEventListener('click', async function () {
    try {
        const res = await fetch('http://localhost:3000/download-folder');

        if (res.ok) {
            console.log(res);
            //main code 
            const blob = await res.blob();
            const url = window.URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = 'project-folder.zip'; 
            a.click();
        }
          else {
            console.error('Failed to fetch zip file:', res.status);
        }

    } catch (error) {
        console.error('Error in fetch request:', error);
    }
});
}
//http://localhost:3000/about
//http://localhost:5000/download
