const express = require('express');
const fs = require('fs'); 
const path = require('path');
const archiver = require('archiver');
const cors = require('cors');

const app = express();
app.use(cors());


const FOLDER_PATH = 'C:/Users/abdus/OneDrive/Desktop/code';  // Folder to be zipped

app.get('/download-folder', (req, res) => {
  
  const zipPath = 'C:/Users/abdus/Downloads/project-folder.zip'; 

  const output = fs.createWriteStream(zipPath);
  const archive = archiver('zip');

  // When the archive is created and closed
  output.on('close', () => {
    res.download(zipPath, 'project-folder.zip', () => {
      fs.unlinkSync(zipPath); // Clean up zip file after sending
    });
  });

  archive.pipe(output);  // Pipe the archive to the write stream
  archive.directory(FOLDER_PATH, false);  // Add folder to the archive
  archive.finalize();  // Finalize the archive creation

  console.log('Folder zip creation started...');
});

app.listen(3000, () => {
  console.log('Server running at http://localhost:3000');
});

//archive 1.pipe 2.directory 3.finalize