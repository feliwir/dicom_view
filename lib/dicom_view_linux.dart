// In order to *not* need this ignore, consider extracting the "web" version
// of your plugin as a separate package, instead of inlining it in the same
// package as the core of your plugin.
// ignore: avoid_web_libraries_in_flutter
import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';

import 'dicom_view_method_channel.dart';

/// A web implementation of the DicomViewPlatform of the DicomView plugin.
class DicomViewLinux extends StatefulWidget {
  @visibleForTesting
  final methodChannel = MethodChannelDicomViewPlatform();

  @override
  State<StatefulWidget> createState() {
    return _DicomViewLinuxState();
  }
}

class _DicomViewLinuxState extends State<DicomViewLinux> {
  int _textureId = 0;
  MethodChannelDicomView? viewChannel;

  @override
  void initState() {
    super.initState();
    widget.methodChannel.createViewChannel().then((value) => {
          setState(() {
            viewChannel = value;
            viewChannel?.setDicomFile(file: 'test.dcm');
          })
        });
  }

  @override
  Widget build(BuildContext context) {
    return FutureBuilder<int?>(
        future: viewChannel?.getTextureId(),
        builder: (context, snapshot) {
          if (snapshot.hasError) {
            return Text('Error: ${snapshot.error}');
          } else if (snapshot.hasData) {
            _textureId = (snapshot.data) ?? 0;
            return Texture(textureId: _textureId);
          }
          return const CircularProgressIndicator();
        });
  }
}
